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
    sudo apt-get install -y libgtk-3-dev
fi
