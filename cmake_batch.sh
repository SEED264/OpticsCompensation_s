#!/bin/sh

if [ $# -gt 0 ]; then
    mode=$1
else
    mode=""
fi

# Move current directory to build directory
script_dir=`dirname $0`
build_dir=$script_dir"/build"
if [ ! -e $build_dir ]; then
    echo "Build dir doesn't exists."
    echo "Creating build dir."
    mkdir -p $script_dir"/build"
fi
cd $script_dir"/build"

# Install dir
cmake_install_path="./bin"

# Launch cmake
case $mode in
"msvc")
    cmake_prefix_path=""

    cmake .. \
        -DCMAKE_PREFIX_PATH=$cmake_prefix_path \
        -DCMAKE_INSTALL_PREFIX=$cmake_install_path
;;

"msys")
    cmake_prefix_path=""

    cmake .. -G"MSYS Makefiles" \
        -DCMAKE_PREFIX_PATH=$cmake_prefix_path \
        -DCMAKE_INSTALL_PREFIX=$cmake_install_path
;;

"clean")
    current_dir_name=`basename \`pwd\``
    if [ $current_dir_name = "build" ]; then
        rm -rf *
    fi
;;

*)
    echo "Error : Invalid option"
;;
esac