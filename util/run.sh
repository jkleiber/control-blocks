#! /bin/bash

# Change to util/ directory and run relative to it
cd "$(dirname "$0")"
CB_BIN="$(pwd)/../build/apps"

if [[ "$OSTYPE" == "msys" ]]; then
    # Windows

    # Set the PYTHONHOME to allow python embedding to work
    # export PYTHONHOME="C:\Users\jklei\.virtualenvs\dynamical-system-9PB9hqtZ\Scripts"
    # export PYTHONHOME="C:\Program Files\WindowsApps\PythonSoftwareFoundation.Python.3.10_3.10.2032.0_x64__qbz5n2kfra8p0"
    export PYTHONPATH="C:\Program Files\WindowsApps\PythonSoftwareFoundation.Python.3.10_3.10.2032.0_x64__qbz5n2kfra8p0"

    # Change to the binary directory and run the app.
    cd ${CB_BIN}
    ./controlblocks.exe
else
    # Linux

    # Change to the binary directory and run the app.
    cd ${CB_BIN}
    ./controlblocks
fi
