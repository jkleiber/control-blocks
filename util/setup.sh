#! /bin/bash

# Install platform dependent tools

# Windows
if [[ "$OSTYPE" == "msys" ]]; then
    pacman -S mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja
else
    # Linux dependencies

    # Install clang-format
    sudo apt install -y clang-format libusb-1.0-0-dev

    # Install gtk dependency for NFD library compilation
    sudo apt-get install -y libgtk-3-dev build-essential gnome-devel

    # Install Boost
    wget -O boost_1_80_0.tar.gz https://boostorg.jfrog.io/artifactory/main/release/1.80.0/source/boost_1_80_0.tar.gz
    tar xzvf boost_1_80_0.tar.gz
    cd boost_1_80_0/
    sudo apt-get install -y g++ python-dev autotools-dev libicu-dev libbz2-dev # build-essential installed earlier
    ./bootstrap.sh --prefix=/usr/local/
    sudo ./b2 install
fi
