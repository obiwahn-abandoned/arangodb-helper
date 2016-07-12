#include <iostream>
#include <cassert>
#include <memory>
#include <string>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <chrono>
#include <vector>
#include <cmath>
#include <cstdlib>

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiredtiger.h>
}

static const char* home;
using cclock  = std::chrono::high_resolution_clock;

bool load_data_from_file(WT_CURSOR* cursor, std::string path){
    int ret = 0;
    std::cout << "adding data from disk" << std::endl;
    std::size_t index = 0;
    std::ifstream stream;
    auto start = cclock::now();
    try {
        stream.open(path);
        if(!stream.is_open()){
            std::cout << "Unable to open stream!";
            return false;
        }

        std::istream_iterator<std::string> stream_it(stream);
        for(;stream_it != std::istream_iterator<std::string>(); ++stream_it ){


            auto key = std::to_string(index++);
            cursor->set_key(cursor, key.c_str());        /* Insert a record. */
            cursor->set_value(cursor, stream_it->c_str());
            ret = cursor->insert(cursor);
            //ret = cursor->reset(cursor);            /* Restart the scan. */

            if(ret != 0){ throw std::logic_error("boom"); }

            if (index % 10000 == 0){
                std::cout << index << std::endl;
            }
        }
    } catch (std::exception const& e) {
        std::cout <<e.what();
        return false;
    }
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(cclock::now() - start);
    std::cout << "loading took " << duration.count() << " seconds" << std::endl;
    return true;
}

std::size_t next_element_all(std::size_t current, std::size_t count, std::size_t prime = std::pow(2,32) -1){
    return (current + prime) % count;
}


std::size_t run_test(WT_CURSOR* cursor, std::size_t count){
    const char * volatile value;
    auto start = cclock::now();
    auto test_count = count / 10;
    int ret = 0;
    std::string data;

    std::cout << "get: " << test_count << " documents" << std::endl;

    std::size_t key = 1;
    for(std::size_t i = 0 ; i < test_count ; ++i){
        ret = cursor->get_key(cursor, std::to_string(i).c_str());
        ret = cursor->get_value(cursor, &value);
        key = next_element_all(key, count);
        ret = cursor->reset(cursor);            /* Restart the scan. */
    }

    auto duration = std::chrono::duration_cast<std::chrono::seconds>(cclock::now() - start);
    return duration.count();
}


int main(){
    auto data_path = std::getenv("OBI_DATA");
    if(!data_path){
        std::cout << "OBI_DATA environment variable is not set!"  << std::endl;
        return 1;
    }


    WT_CONNECTION *conn;
    WT_CURSOR *cursor;
    WT_SESSION *session;
    const char *key, *value;
    int ret;

    /*
     * Create a clean test directory for this run of the test program if the
     * environment variable isn't already set (as is done by make check).
     */
    if (getenv("WIREDTIGER_HOME") == NULL) {
            home = "WT_HOME";
            ret = system("rm -rf WT_HOME && mkdir WT_HOME");
    } else {
            home = NULL;
    }

    /* Open a connection to the database, creating it if necessary. */
    if ((ret = wiredtiger_open(home, NULL, "create", &conn)) != 0 ||
        (ret = conn->open_session(conn, NULL, NULL, &session)) != 0) {
            fprintf(stderr, "Error connecting to %s: %s\n",
                home, wiredtiger_strerror(ret));
            return (ret);
    }

    ret = session->create(session, "table:access10", "key_format=S,value_format=S");
    ret = session->open_cursor(session, "table:access10", NULL, NULL, &cursor);

    load_data_from_file(cursor, data_path);

    std::cout << "first  run warum up: " << std::endl
              << "seconds taken: " << run_test(cursor, 100000000) << std::endl;
    std::cout << "second - real test:  " << std::endl
              << "seconds taken: " << run_test(cursor, 100000000) << std::endl;

    ret = cursor->close(cursor);
    ret = conn->close(conn, NULL);
    return (ret);
}
