#!/bin/sh
# Configure OSX version into build/osx for Xcode IDE
mkdir -p osx
cd osx
cmake -G Xcode ../..
cd ..
