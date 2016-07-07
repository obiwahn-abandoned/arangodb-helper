#include <cstdlib>
#include <fstream>
#include <string>
#include <iostream>

std::string gen_random(int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    auto s = std::string(len, ' ' );
    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[std::rand() % (sizeof(alphanum) - 1)];
    }

    return s;
}

int main(){

    std::size_t file_size_gb = 10;
    std::size_t item_size = 100;

    file_size_gb *= 1000000000;
    std::size_t num_items=file_size_gb / item_size;
    std::cout << num_items << std::endl;
    std::ofstream out("data.txt");

    for(std::size_t i = 0; i < num_items; ++i){
        out << gen_random(item_size) << std::endl;
    }
}
