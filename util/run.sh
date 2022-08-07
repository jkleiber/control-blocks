#! /bin/bash

# Change to scripts directory and run relative to it
cd "$(dirname "$0")"
CB_BIN="$(pwd)/../build/apps"

# Change to the robot binaries directory and run the simulator code
cd ${CB_BIN}
./controlblocks