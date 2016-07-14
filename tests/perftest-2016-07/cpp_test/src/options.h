#pragma once
#include <string>
#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

struct options {
    std::string data_source;
    std::string db_directory;
    std::string database;
    std::string test;
    bool drop_data;
};

inline options parse_options(int ac, char* av[]){
    options opts;

    po::options_description desc("Allowed Options");
    desc.add_options()
        ( "help,h",
          "produce help message" )
        ( "source,s",
          po::value<std::string>(&opts.data_source),
          "path to data source" )
        ( "dbdir,v",
          po::value<std::string>(&opts.db_directory),
          "path to storage directory")
        ( "database,d",
          po::value<std::string>(&opts.database)->default_value("arangodb"),
          "database to use")
        ( "test,t",
          po::value<std::string>(&opts.test)->default_value("all"),
          "test to run")
        ( "drop,r",
          po::value<bool>(&opts.drop_data)->default_value(false)->implicit_value(true),
          "drop database")
    ;
    po::positional_options_description p;

    po::variables_map vm;
    po::store(po::command_line_parser(ac, av).options(desc)
                                             .positional(p)
                                             .run()
             ,vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        std::exit(1);
    }

    return opts;

}
