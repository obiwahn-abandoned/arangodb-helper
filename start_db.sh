#!/bin/bash

source_dir="$HOME/work/arangodb"
build_dir="$HOME/work/arangodb/build"
data_dir="$HOME/work/arangodb-data/"
data_conf="$HOME/work/arangodb-conf/"

cd "$source_dir"
clear

"./build/bin/arangod" \
    --configuration none \
    --javascript.startup-directory js \
    --javascript.app-path "js/apps" \
    --server.endpoint tcp://127.0.0.1:8529 \
    --database.directory "$data_dir"
rv=$?


if (( rv != 0 )); then
    echo
    echo
    echo
    echo
    echo "failed to start database!"
fi

exit $rv
