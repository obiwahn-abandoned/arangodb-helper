#!/bin/bash

echo "${BASH_SOURCE}"
source_path="$(realpath -s "${BASH_SOURCE[0]}")"
source_dir="$(dirname "$source_path")"

source_dir="$(readlink -f $(pwd))"


if [[ -z $source_dir ]]; then
    echo "could not resolve source_dir"
    exit 1
fi

if ! [[ -f $source_dir/VERSION ]] || ! [[ -d $source_dir/arangod ]]; then
    echo "'$source_dir' is not a source directory"
    exit 1
fi

build_type="RelWithDebInfo"

args=()

cmake_jemalloc="OFF"
cmake_maintainer_mode="ON"
cmake_enterprise="OFF"


asan=false
tsan=false
perf=false
switch_only=false
delete_source_dir=false

build=false

while [[ -n $1 ]]; do
    case $1 in
        r:RelWith*)
            build_type="RelWithDebInfo"
            shift
        ;;
        r:Release)
            build_type="Release"
            shift
        ;;
        r:Debug)
            build_type="Debug"
            shift
        ;;
        -d|--delete)
            delete_source_dir=true
            shift
        ;;
        -a|--asan)
            asan=true
            tsan=false
            cmake_jemalloc=false
            shift
        ;;
        -p| --perf)
            perf=true
            shift
        ;;
        -t|--tsan)
            asan=false
            tsan=true
            cmake_jemalloc=false
            shift
        ;;
        -nj|--no-jemalloc)
            cmake_jemalloc="OFF"
            shift
        ;;
        -s|--switch-only)
            switch_only=true
            shift
        ;;
        -e|--enterprise)
            cmake_enterprise="ON"
            shift
        ;;
        -b|--build)
            build=true
            shift
        ;;
        --no-maint*)
            cmake_maintainer_mode="OFF"
            shift
        ;;
        *)
            args+=( "$1" )
            shift
        ;;
    esac

done
echo "extra args: ${args[@]}"

compiler=gcc

mkdir -p "${source_dir}-build"
build_dir="${source_dir}-build/${build_type}"

if $asan; then
    build_dir="${build_dir}-asan"
fi

echo "source in: $source_dir"

echo "update <build_dir>/current line: '${source_dir}-build/current'"
rm "${source_dir}-build/current"
ln -s "$build_dir" "${source_dir}-build/current"

echo "update <source>/build link: '${source_dir}/build'"
rm "${source_dir}/build"
ln -s "$build_dir" "${source_dir}/build"

if $switch_only; then
    exit 0
fi

if $delete_source_dir; then
  echo "delete old build in '$build_dir'"
  rm -fr "$build_dir"
fi

if ! [ -d $build_dir ]; then
    echo "create build dir: '$build_dir'"
    mkdir -p $build_dir || { echo "could not create build dir"; exit 1; }
fi
echo "change into build dir: $build_dir"
cd $build_dir || { echo "could not enter build dir"; exit 1; }


case "$compiler" in
    *clang*)
        flags="$debug_flags"
        cxx=/usr/bin/clang++-4.0
        cc=/usr/bin/clang-4.0
        unset LD_LIBRARY_PATH
    ;;
    *clanghead*)
        flags="$debug_flags"
        cxx=/usr/local/bin/clang++
        cc=/usr/local/bin/clang
        unset LD_LIBRARY_PATH
    ;;
    *gcc*)
        flags=""
        flags_asan="-fsanitize=address -fsanitize=undefined -fno-sanitize=alignment -fno-omit-frame-pointer -fno-sanitize=vptr -pthread"
        flags_tsan="-fsanitize=undefined -fno-sanitize=alignment -fno-omit-frame-pointer -fno-sanitize=vptr -fsanitize=thread"
        cxx="/usr/bin/g++"
        cc="/usr/bin/gcc"
        ;;
    *gcchead*)
        #flags_asan="-fsanitize=address -fsanitize=undefined -fno-sanitize=alignment -fno-sanitize=vptr"
        flags="$debug_flags -lpthread -gdwarf-4 "
        cxx="$HOME/.bin/g++-6"
        cc="$HOME/.bin/gcc-6"
        export LD_LIBRARY_PATH=/opt/gcc_stable_2016-07-06/lib64
    ;;
esac

if $asan; then
    flags="$flags_asan $flags"
fi

if $tsan; then
    flags="$flags_tsan $flags"
fi

if $perf; then
    flags="$flags -fno-omit-frame-pointer"
fi

cxx_flags="$flags"

sleep 3
set -x
CXX=${CXX-$cxx} \
CC=${CC-$cc} \
CFLAGS="$flags" \
CXXFLAGS="$cxx_flags" \
    cmake -DCMAKE_BUILD_TYPE=$build_type \
          -DUSE_MAINTAINER_MODE="$cmake_maintainer_mode" \
          -DUSE_FAILURE_TESTS=ON \
          -DUSE_ENTERPRISE=$cmake_enterprise \
          -DUSE_JEMALLOC="$cmake_jemalloc" \
          -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
          -DCMAKE_INSTALL_PREFIX=~/arangodb-install \
          ${args[@]} \
          $source_dir


if [[ $? ]]; then
    echo "configuration successful"
    pwd
else
    echo "failed to configure"
    pwd
fi

if $build; then
    make -j $(nproc)
fi
