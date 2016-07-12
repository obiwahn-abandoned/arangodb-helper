cmake_minimum_required(VERSION 2.8.12)
project(rocksdb-perf CXX C)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++-14")

find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)

add_executable(rocks-test rocksdb_test.cpp)
target_link_libraries(rocks-test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/rocksdb/librocksdb.a
    ${ZLIB_LIBRARIES}
    ${BZIP2_LIBRARIES}
    snappy
    ${CMAKE_THREAD_LIBS_INIT}
)

target_include_directories(rocks-test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/rocksdb/include)

