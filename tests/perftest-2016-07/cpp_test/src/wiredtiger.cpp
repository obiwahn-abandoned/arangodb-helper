#include "wiredtiger.h"
#include "shared.h"

#include <chrono>
#include <boost/filesystem.hpp>
#include <fstream>
namespace fs = boost::filesystem;
using cclock  = std::chrono::high_resolution_clock;
using namespace std::literals;
using std::string;


static void check_exit_on_fail(int ret){
    if(ret != 0){
        std::cout << "exit: " << __LINE__ << wiredtiger_strerror(ret) << std::endl;
        std::exit(ret);
    }
}

static const char *home;

WT_CONNECTION *conn;
WT_CURSOR *cursor;
WT_SESSION *session;
const char *key, *value;

static std::size_t
run_test(std::size_t(*next_key)(std::size_t, std::size_t)
        ,std::size_t count
        ){

    auto start = cclock::now();
    auto test_count = count / 10;
    int ret = 0;

    std::string data;
    std::cout << "get: " << test_count << " documents" << std::endl;

    std::size_t key = 1;
    for(std::size_t i = 0 ; i < test_count ; ++i){
        std::string key_string = std::to_string(key);
        cursor->set_key(cursor, key_string.c_str());
        ret = cursor->search(cursor);
        if (ret != 0){
            std::cout << "key " << key << " not found " << std::endl ;
            std::cout << "this is unexpected! exiting!" << std::endl;
            std::exit(42);
        } else {
            ret = cursor->get_value(cursor, &value);
            if (ret != 0){
                std::cout << " could not get value" << std::endl ;
            }
            if (i%1000000 == 0){
                std::cout << key << " -- " << value << std::endl;
            }
            if (i < 10){
                    std::cout << key << " -- " << value << std::endl;
            }
        }
        if (i%10000 == 0){
            std::cout << " " << i << " ";
        }
        ret = cursor->reset(cursor);
        key = next_key(key, count);
    }

    auto duration = std::chrono::duration_cast<std::chrono::seconds>(cclock::now() - start);
    return duration.count();
}

int run_tiger(options opts)
{
	/*! [access example connection] */
	int ret;

    //check dir
    fs::path db_path(opts.db_directory);
    if(!fs::is_directory(db_path)){
        std::cout << "db_directory not valid" << std::endl;
        std::exit(1);
    }

    home = opts.db_directory.c_str();

	/* Open a connection to the database, creating it if necessary. */
	if ((ret = wiredtiger_open(home, NULL, "create", &conn)) != 0 ||
	    (ret = conn->open_session(conn, NULL, NULL, &session)) != 0) {
		fprintf(stderr, "Error connecting to %s: %s\n",
		    home, wiredtiger_strerror(ret));
		return (ret);
	}

    //move check to general part
    fs::path data_path(opts.data_source);
    std::string collection_name;
    if(!fs::is_regular_file(data_path)){
        std::cout << "not a regular file!" << std::endl;
        std::exit(1);
    } else {
        //collection_name = data_path.stem().string();
        collection_name = "data30"s;
        std::cout << "collection name: " << collection_name << std::endl;
    }
    auto table = "table:"s + collection_name;

    if(opts.drop_data){
        ret = session->drop(session, table.c_str(), "force=true");
        check_exit_on_fail(ret);
    }

	ret = session->create(session
                         ,table.c_str()
	                     ,"key_format=S,value_format=S");
    check_exit_on_fail(ret);
	/*! [access example table create] */

	/*! [access example cursor open] */
	ret = session->open_cursor(session, table.c_str(), NULL, NULL, &cursor);
    check_exit_on_fail(ret);



    //data has to be read again
    if(opts.drop_data){

        std::cout << "adding data from disk - file: " << data_path << std::endl;
        std::size_t index = 0;
        std::ifstream stream;
        auto start = cclock::now();
        try {
            stream.open(data_path.string());
            if(!stream.is_open()){
                std::cout << "Unable to open stream!" << std::endl;
                return false;
            }

            ret = session->begin_transaction(session, NULL);
            check_exit_on_fail(ret);
            std::istream_iterator<std::string> stream_it(stream);
            for(;stream_it != std::istream_iterator<std::string>(); ++stream_it ){


                auto key = std::to_string(index++);
                cursor->set_key(cursor, key.c_str());        /* Insert a record. */
                cursor->set_value(cursor, stream_it->c_str());
                ret = cursor->insert(cursor);
                //ret = cursor->reset(cursor);            /* Restart the scan. */

                check_exit_on_fail(ret);

                if (index % 100000 == 0){
                    ret = session->commit_transaction(session, NULL);
                    check_exit_on_fail(ret);
                    ret = session->begin_transaction(session, NULL);
                    check_exit_on_fail(ret);
                    std::cout << index << std::endl;
                }
            }
        } catch (std::exception const& e) {
            std::cout << e.what() << std::endl;
            return false;
        }
        ret = session->commit_transaction(session, NULL);
        check_exit_on_fail(ret);
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(cclock::now() - start);
        std::cout << "loading from file took " << duration.count() << " seconds" << std::endl;
    }
    ret = cursor->close(cursor);
    check_exit_on_fail(ret);
	ret = session->open_cursor(session, table.c_str(), NULL, NULL, &cursor);
    check_exit_on_fail(ret);

    if (opts.test.compare("all") != 0){
    std::cout << "algorithm:" << opts.test << std::endl;
    std::cout << "first  run warum up: " << std::endl
              << "seconds taken: " << run_test(get_next_key_algorithm(opts.test) , 300*1000*1000) << std::endl;
    std::cout << "second - real test:  " << std::endl
              << "seconds taken: " << run_test(get_next_key_algorithm(opts.test) , 300*1000*1000) << std::endl;
    }

    ret = session->close(session, NULL);
    check_exit_on_fail(ret);
    ret = conn->close(conn, NULL);
    check_exit_on_fail(ret);

	return (ret);
}
