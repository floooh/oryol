#!/bin/sh
# Configure Linux Release version into build/linux
mkdir -p linux
cd linux
cmake -G "Ninja" -DCMAKE_BUILD_TYPE="Release" ../..
cd ..

