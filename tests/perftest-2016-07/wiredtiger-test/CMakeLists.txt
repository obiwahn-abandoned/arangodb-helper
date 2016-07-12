cmake_minimum_required(VERSION 2.8.12)
project(wiredtiger-perf CXX C)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)

add_executable(wired-test wiredtiger_test.cpp)
target_link_libraries(wired-test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/wiredtiger/.libs/libwiredtiger.a
    ${ZLIB_LIBRARIES}
    ${BZIP2_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    -ldl
)

target_include_directories(wired-test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/wiredtiger/)

