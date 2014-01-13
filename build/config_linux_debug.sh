#!/bin/sh
# Configure Linux Debug version into build/linux
mkdir -p linux
cd linux
cmake -G "Ninja" -DCMAKE_BUILD_TYPE="Debug" ../..
cd ..


