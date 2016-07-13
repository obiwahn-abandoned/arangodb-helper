#include "options.h"
#include <iostream>
#include <iomanip>


int main(int argc, char* argv[]){
    options opts = parse_options(argc, argv);
    std::cout << "database:           "  << opts.database << std::endl;
    std::cout << "database directory: " << opts.db_directory << std::endl;
    std::cout << "drop database:      " << std::boolalpha << opts.drop_data << std::endl;
    std::cout << "source:             " << opts.data_source << std::endl;
    std::cout << "test:               " << opts.test << std::endl;

}
