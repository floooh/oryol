#!/bin/sh
# Configure emscripten Release version for make into build/emsc
cd emsc
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE="Release" -DCMAKE_TOOLCHAIN_FILE="../../cmake/emscripten.toolchain.cmake" ../..
cd ..
