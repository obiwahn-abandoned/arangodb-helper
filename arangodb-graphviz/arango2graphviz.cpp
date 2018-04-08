#include <fstream>
#include <iostream>

#include <tao/json.hpp>
#include <arango-graphviz.hpp>


int main(int argc, char *argv[]){
    if(argc < 2){
        return 1;
    }
    std::string filename(argv[1]);
    std::ifstream is(filename);

    auto value = tao::json::from_stream(is,nullptr);
    auto graphs = arangodb::gparser::parse(value);
    arangodb::gparser::to_graphviz_stream(std::cout, graphs);
    return 0;
}
