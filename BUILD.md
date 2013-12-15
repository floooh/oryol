This file describes how to build Oryol on the supported platforms.

Internal build system details are described in docs/buildsystem.md

### OSX ###

On OSX, Oryol is built with Xcode and supports both bundled applications as well as simple command line applications.

1. install Xcode from the app shop (tested with Xcode 5)
2. install cmake, for instance with 'brew install cmake' (at least cmake version 2.8 is required), the 
homebrew package manager can be installed from http://brew.sh/
3. start cmake-gui
4. point the field "Where is the source code:" to the oryol root directory (e.g. /Users/[user]/oryol)
5. point the field "Where to build the binaries:" to the build/osx subdirectory (e.g. /Users/[user]/oryol/build/osx)
6. press the Configure button
7. now you can optionally change build settings (for instead switch between Debug and Release build mode)
8. press Configure until there are no more entries with red background
9. press Generate to create Xcode project files
10. open the generated Xcode project file (e.g. /Users/[user]/oryol/build/osx/oryol.xcodeproj)
11. select the "hello" target, and compile/run it with Cmd+R, a "Hello World!" should show up in the Xcode output console

There are also shortcut config scripts in the build subdirectory:

1. cd ~/oryol/build
2. ./config_osx_xcode.sh

This will create an Xcode project file under oryol/build/osx with default compile settings.

### Linux ###

On Linux, Oryol is built from the command line with make and GCC.

1. make sure that GCC and make are in the path
2. cd to oryol/build
3. execute ./build_linux_debug_make.sh
4. cd to oryol/build/linux
5. execute 'make hello', this should compile the hello world sample
6. go to oryol/bin/linux, there should be a 'hello_debug' executable, running this should display 'Hello World!'
