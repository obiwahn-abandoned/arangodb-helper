#!/bin/bash
export LD_LIBRARY_PATH=/opt/gcc_stable_2016-07-06/lib64
source ../scripts.conf

echo "\$source_dir: $source_dir"
echo "\$build_dir: $build_dir"
echo "\$binary_dir: $binary_dir"
echo "\$data_dir: $data_dir"
echo "\$helper_dir: $helper_dir"

cd "$source_dir"
