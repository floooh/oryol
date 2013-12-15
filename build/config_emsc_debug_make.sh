#!/bin/sh
# Configure emscripten Debug version for make into build/emsc
cd emsc
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE="Debug" -DCMAKE_TOOLCHAIN_FILE="../../cmake/emscripten.toolchain.cmake" ../..
cd ..

