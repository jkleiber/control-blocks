#! /bin/bash

# Find compile script working directory
cd "$(dirname "$0")"
UTIL_DIR="$(pwd)"
COMPILE_WD="$(pwd)/../build"
CONFIG_WD="$(pwd)/../config"

# Change to build directory
cd $COMPILE_WD

# Use platform dependent build tools.
# Windows - Use msys and ninja to compile
if [[ "$OSTYPE" == "msys" ]]; then
    # Build the code without the Gazebo simulation.
    cmake .. -G Ninja -DCOMPILE_SIMULATION=0
    ninja
else
    # Linux - use cmake and make

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
fi