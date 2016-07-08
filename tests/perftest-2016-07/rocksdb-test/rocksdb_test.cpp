#include <iostream>
#include <cassert>
#include <memory>
#include <string>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <chrono>
#include <vector>
#include "rocksdb/db.h"
#include <cmath>

bool load_data_from_file(rocksdb::DB* db, std::string path){
    std::cout << "adding data from disk" << std::endl;
    std::size_t index = 0;
    std::ifstream stream;
    rocksdb::Status status;
    try {
        stream.open(path);
        if(!stream.is_open()){
            std::cout << "Unable to open stream!";
            return false;
        }

        std::istream_iterator<std::string> stream_it(stream);
        for(;stream_it != std::istream_iterator<std::string>(); ++stream_it ){
            auto key = std::to_string(index++);
            status = db->Put(rocksdb::WriteOptions(), key, *stream_it);
            if(!status.ok() ){ throw std::logic_error("boom"); }
            if (index % 10000 == 0){
                std::cout << index << std::endl;
            }
        }
    } catch (std::exception const& e) {
        std::cout <<e.what();
        return false;
    }
    return true;
}

std::size_t next_element_all(std::size_t current, std::size_t count, std::size_t prime = std::pow(2,32) -1){
    return (current + prime) % prime;
}


std::size_t run_test(rocksdb::DB* db, std::size_t count){
    using clock  = std::chrono::high_resolution_clock;
    auto start = clock::now();
    auto test_count = count / 10;
    rocksdb::Status status;
    std::string data;

    std::cout << "get: " << test_count << " documents";
    auto read_options = rocksdb::ReadOptions();

    std::size_t key = 1;
    for(std::size_t i = 0 ; i < test_count ; ++i){
        status =  db->Get(read_options, std::to_string(i), &data);
        key = next_element_all(key, count);
    }

    auto duration = std::chrono::duration_cast<std::chrono::seconds>(clock::now() - start);

    return duration.count();
}


int main(){
    rocksdb::DB* db = nullptr;
    rocksdb::Options options;
    options.create_if_missing = true;
    std::cout << "loading database" << std::endl;
    rocksdb::Status status = rocksdb::DB::Open(options, "/home/arango/perf-test-obi/rocksdb-test/tmp/testdb", &db);
    assert(status.ok());

    bool load_from_disk = false;
    if (load_from_disk){
        if (!load_data_from_file(db, "/home/arango/perf-test-obi/data.txt")){
            return 1;
        }
    }

    std::cout << "first  run warum up: " << run_test(db, 100000000) << std::endl;
    std::cout << "second - real test:  " << run_test(db, 100000000) << std::endl;

    delete db;
    return 0;
}
