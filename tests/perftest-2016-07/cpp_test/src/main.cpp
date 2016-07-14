#include "options.h"
#include "wiredtiger.h"
#include <iostream>
#include <iomanip>
#include "shared.h"
#include <string>


int main(int argc, char* argv[]){
    options opts = parse_options(argc, argv);




    std::cout << "database:           "  << opts.database << std::endl;
    std::cout << "database directory: " << opts.db_directory << std::endl;
    std::cout << "drop database:      " << std::boolalpha << opts.drop_data << std::endl;
    std::cout << "source:             " << opts.data_source << std::endl;
    std::cout << "test:               " << opts.test << std::endl;

    if(opts.database.compare("arangodb") == 0){
        std::cout << "Test not impletmened for ArangoDB!!" << std::endl;
    }
    else if(opts.database.compare("wiredtiger") == 0){
        run_tiger(opts);
    }
    else if(opts.database.compare("rocksdb") == 0){
        std::cout << "Test not impletmened for RocksDB!!" << std::endl;
    } else {
        std::cout << "No valid Database given!!" << std::endl;
    }
}
