#!/bin/sh
# Configure PNaCl Debug version for make into build/pnacl
mkdir -p pnacl
cd pnacl
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE="Debug" -DCMAKE_TOOLCHAIN_FILE="../../cmake/pnacl.toolchain.cmake" ../..
cd ..

