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
#include <cstdlib>

using cclock = std::chrono::high_resolution_clock;

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

    	auto start = cclock::now();
        std::istream_iterator<std::string> stream_it(stream);
        for(;stream_it != std::istream_iterator<std::string>(); ++stream_it ){
            auto key = std::to_string(index++);
            status = db->Put(rocksdb::WriteOptions(), key, *stream_it);
            if(!status.ok() ){ throw std::logic_error("boom"); }
            if (index % 10000 == 0){
                std::cout << index << std::endl;
            }
        }
    	auto duration = std::chrono::duration_cast<std::chrono::seconds>(cclock::now() - start);
    	std::cout << "loading of data form file " << duration.count() << std::endl;
    } catch (std::exception const& e) {
        std::cout <<e.what();
        return false;
    }
    return true;
}

std::size_t run_test(rocksdb::DB* db, std::size_t(*next_key)(std::size_t, std::size_t), std::size_t count){
    auto start = cclock::now();
//auto test_count = count / 10;
    auto test_count = count;
    rocksdb::Status status;
    std::string data;

    std::cout << "get: " << test_count << " documents" << std::endl;
    auto read_options = rocksdb::ReadOptions();

    std::size_t key = 1;
    for(std::size_t i = 0 ; i < test_count ; ++i){
        status =  db->Get(read_options, std::to_string(key), &data);
        key = next_key(key, count);

	if(i<10){ std::cout << key <<  " -- " << data << std::endl; }
    }


    auto duration = std::chrono::duration_cast<std::chrono::seconds>(cclock::now() - start);

    return duration.count();
}

std::size_t next_key_max_all(std::size_t current, std::size_t count){
    return (current + 47) % count;
}

std::size_t next_key_max_1(std::size_t current, std::size_t count){
    return (current + 47 * 100) % count;
}


int main(){
    auto data_path = std::getenv("OBI_DATA");
    if(!data_path){
        std::cout << "OBI_DATA environment variable is not set!"  << std::endl;
        return 1;
    }

    rocksdb::DB* db = nullptr;
    rocksdb::Options options;
    options.create_if_missing = true;
    std::cout << "loading database" << std::endl;
    rocksdb::Status status = rocksdb::DB::Open(options, std::string(data_path) + std::string("/rocksdb-data"), &db);
    assert(status.ok());

    bool load_from_disk = false;
    if (load_from_disk){
        if (!load_data_from_file(db, std::string(data_path) + std::string("/data30GB.txt"))){
            return 1;
        }
    }

    std::size_t number_of_requests = 300 * 1000 *1000;

    std::cout << "next_key_max_all:" << std::endl;
    std::cout << "first  run warum up: " << std::endl
              << "seconds taken: " << run_test(db, &next_key_max_all, number_of_requests) << std::endl;
    std::cout << "second - real test:  " << std::endl
              << "seconds taken: " << run_test(db, &next_key_max_all, number_of_requests) << std::endl;
    std::cout << "third - real test:  " << std::endl
              << "seconds taken: " << run_test(db, &next_key_max_all, number_of_requests) << std::endl;


    //std::cout << std::endl << "next_key_max_2:" << std::endl;
    //std::cout << "first  run warum up: " << std::endl
    //          << "seconds taken: " << run_test(db, &next_key_max_1 ,300000000) << std::endl;
    //std::cout << "second - real test:  " << std::endl
    //          << "seconds taken: " << run_test(db, &next_key_max_1, 300000000) << std::endl;

    delete db;
    return 0;
}
