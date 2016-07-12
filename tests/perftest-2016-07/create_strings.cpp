#include <cstdlib>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>

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

void create_data(std::size_t& file_size_gb){
    std::size_t item_size = 100;
    std::size_t file_size_byte = file_size_gb * 1000000000;
    std::size_t num_items = file_size_byte / item_size;
    std::cout << num_items << std::endl;
    std::string out_file = "data-";
    out_file += std::to_string(file_size_gb);
    out_file += "GB.txt";
    std::ofstream out(out_file);

    for(std::size_t i = 0; i < num_items; ++i){
        out << gen_random(item_size) << std::endl;
    }
}


int main(){
    std::vector<std::size_t> sizes = {10, 30};
    for(auto& s : sizes){
        create_data(s);
    }
}
