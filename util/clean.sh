#! /bin/bash

# Find compile script working directory
cd "$(dirname "$0")"
UTIL_DIR="$(pwd)"
COMPILE_WD="$(pwd)/../build"
CONFIG_WD="$(pwd)/../config"

# Change to build directory
cd $COMPILE_WD

# Use platform dependent build tools.
# Windows - Use msys Ninja
if [[ "$OSTYPE" == "msys" ]]; then
    ninja -t clean
    if [ $? -ne 0 ]
    then
        echo "Error: CMake clean failed!"
        exit 1
    fi
else
    # Linux - use make

    # CMake the code, exit on failure
    make clean
    if [ $? -ne 0 ]
    then
        echo "Error: CMake clean failed!"
        exit 1
    fi
fi