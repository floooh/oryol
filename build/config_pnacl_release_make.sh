#!/bin/sh
# Configure PNaCl Release version for make into build/pnacl
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE="Release" -DCMAKE_TOOLCHAIN_FILE="../../cmake/pnacl.toolchain.cmake" ../../code
