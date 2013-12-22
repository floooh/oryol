#!/bin/sh
# Configure Linux Debug version into build/linux
mkdir -p linux
cd linux
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE="Debug" -DORYOL_UNITTESTS="ON" ../..
cd ..


