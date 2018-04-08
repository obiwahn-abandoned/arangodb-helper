#!/bin/bash

echo "${BASH_SOURCE}"
source_path="$(realpath -s "${BASH_SOURCE[0]}")"
source_dir="$(dirname $source_path)"

if [[ -z $source_dir ]]; then
    echo "could not resolve source_dir"
    exit 1
fi

build_type="RelWithDebInfo"

args=()

cjemalloc="ON"
enterprise="OFF"
casan=false
switch_only=false
delete_source_dir=false

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
            casan=true
            cjemalloc=false
            shift
        ;;
        -nj|--no-jemalloc)
            cjemalloc=false
            shift
        ;;
        -s|--switch-only)
            switch_only=true
            shift
        ;;
        -e|--enterprise)
            enterprise="ON"
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

if $casan; then
    build_dir="${build_dir}-asan"
fi

echo "source in: $source_dir"

echo "update symlinks"
rm "${source_dir}-build/current"
ln -s "$build_dir" "${source_dir}-build/current"
rm "${source_dir}/build"
ln -s "$build_dir" "${source_dir}/build"

if $switch_only; then
    exit 0
fi

if $delete_source_dir; then
  echo "delete old build"
  rm -fr "$build_dir/"*
fi

echo "create dir if neccessary"
[ -d $build_dir ] || mkdir -p $build_dir
echo "change into build dir: $build_dir"
cd $build_dir || exit


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
        asan="-fsanitize=address -fsanitize=undefined -fno-sanitize=alignment -fno-omit-frame-pointer -fno-sanitize=vptr" 
        cxx="/usr/bin/g++"
        cc="/usr/bin/gcc"
        ;;
    *gcchead*)
        #asan="-fsanitize=address -fsanitize=undefined -fno-sanitize=alignment -fno-sanitize=vptr"
        flags="$debug_flags -lpthread -gdwarf-4 $asan"
        cxx="$HOME/.bin/g++-6"
        cc="$HOME/.bin/gcc-6"
        export LD_LIBRARY_PATH=/opt/gcc_stable_2016-07-06/lib64
    ;;
esac

if $casan; then
    flags="$asan"
fi

cxx_flags="$flags"
#CFLAGS="$flags" \

set -x
CXX=$cxx \
CC=$cc \
CXXFLAGS="$cxx_flags" \
    cmake -DCMAKE_BUILD_TYPE=$build_type \
          -DUSE_MAINTAINER_MODE=ON \
          -DUSE_BOOST_UNITTESTS=ON \
          -DUSE_FAILURE_TESTS=ON \
          -DUSE_ENTERPRISE=$enterprise \
          -DUSE_JEMALLOC="$cjemalloc" \
          -DUSE_IRESEARCH=ON \
          -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
          $source_dir
set +x
          #-DUSE_SSL=ON \


if [[ $? ]]; then
    echo "configuration successful"
    pwd
else
    echo "failed to configure"
    pwd
fi
