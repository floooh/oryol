#!/bin/sh
# Configure OSX version into build/osx for Xcode IDE
cd osx
cmake -G Xcode ../..
cd ..
