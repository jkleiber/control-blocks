#! /bin/bash

# Find compile script working directory
cd "$(dirname "$0")"
UTIL_DIR="$(pwd)"
COMPILE_WD="$(pwd)/../build"
CONFIG_WD="$(pwd)/../config"

# Change to build directory
cd $COMPILE_WD

# CMake the code, exit on failure
cmake ..
if [ $? -ne 0 ]
then
    echo "Error: CMake failed!"
    exit 1
fi

# Make the executable, exit on failure
make
if [ $? -ne 0 ]
then
    echo "Error: make failed!"
    exit 2
fi