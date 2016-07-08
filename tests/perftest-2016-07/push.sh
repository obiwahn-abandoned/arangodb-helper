#!/bin/bash


files=(
    "arangodb_perf_test.js"
    "create_strings.cpp"
    "rocksdb-test/CMakeLists.txt"
    "rocksdb-test/rocksdb_test.cpp"
)
for file in "${files[@]}"; do
    scp "$file" "amazon:arangodb-helper/tests/perftest-2016-07/$file"
done
