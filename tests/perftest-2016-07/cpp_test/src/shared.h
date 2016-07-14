#pragma once
#include <string>
#include <map>

inline std::size_t next_key_max_all(std::size_t current, std::size_t count){
    return (current + 47) % count;
}

inline std::size_t next_key_max_1(std::size_t current, std::size_t count){
    return (current + 47 * 100) % count;
}

inline auto get_next_key_algorithm(std::string algo){
    static std::map<std::string,std::size_t(*)(std::size_t,std::size_t)> map =
    { { "max_1", &next_key_max_1}
    , { "max_all", &next_key_max_all}
    };

    auto fun = map[algo];
    if(!fun){
        std::cout << "Algorithm not found: " << algo << std::endl;
        std::exit(1);
    }
    return fun;
}
