cmake_minimum_required(VERSION 3.9.0 FATAL_ERROR)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set project and version
project(arango-graphviz VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# define a header-only library
add_library(arango-graphviz INTERFACE)
target_include_directories(arango-graphviz INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# add taocpp-json
add_subdirectory(3rdParty/json EXCLUDE_FROM_ALL)
add_subdirectory(3rdParty/velocypack EXCLUDE_FROM_ALL)
add_subdirectory(3rdParty/libobi EXCLUDE_FROM_ALL)
target_link_libraries(arango-graphviz INTERFACE
    taocpp-json
    velocypack
    obi
)

# add c++programm
add_executable(arango2graphviz
    arango2graphviz.cpp
)
target_link_libraries(arango2graphviz PUBLIC
    arango-graphviz
)

# This copies the compile commands to the source dir.
# There they can be used to repeat parts of compile
# steps generating an AST. This AST can in turn be used
# for meaningful auto-completion in editors without
# heuristics, dice rolling and other guessing.

if( EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json" )
  message(STATUS "copy compile_commands.json")

  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
      ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
  )
endif()
## # testing
## enable_testing ()
## option (TAOCPP_JSON_BUILD_TESTS "Build test programs" ON)
## if (TAOCPP_JSON_BUILD_TESTS)
##   add_subdirectory (src/test/json)
## endif ()
## 
## # examples
## option (TAOCPP_JSON_BUILD_EXAMPLES "Build example programs" ON)
## if (TAOCPP_JSON_BUILD_EXAMPLES)
##   add_subdirectory (src/example/json)
## endif ()
## 
## # installation directories
## set (TAOCPP_JSON_INSTALL_INCLUDE_DIR "include" CACHE STRING "The installation include directory")
## set (TAOCPP_JSON_INSTALL_DOC_DIR "share/doc/tao/json" CACHE STRING "The installation doc directory")
## 
## # install
## install (DIRECTORY include/ DESTINATION ${TAOCPP_JSON_INSTALL_INCLUDE_DIR})
## install (FILES LICENSE DESTINATION ${TAOCPP_JSON_INSTALL_DOC_DIR})
