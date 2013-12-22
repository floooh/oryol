This file describes how to build the Oryol "hello world" minimal test program for various platforms.

More detailed build configuration settings are available by using the cmake-gui tool.

Internal build system details are described in docs/buildsystem.md

### OSX ###

On OSX, Oryol is built with Xcode and supports both bundled applications as well as simple command line applications.

1. make sure that Xcode and cmake are installed (Xcode is available from the app store, for cmake I recommend homebrew: http://brew.sh)
2. cd to 'oryol/build'
3. execute './config_osx_xcode.sh'
4. in Xcode open the project 'oryol/build/osx/oryol.xcodeproj'
5. select the 'hello' build target and hit Cmd+R, this will compile and run the minimal "hello world" test
6. a "Hello World!" string should show up in Xcode's output console

### Linux ###

On Linux, Oryol is built from the command line with make and GCC.

At least gcc 4.8.1 is needed to compile oryol.

The vagrant/linux subdirectory contains a vagrant configuration with
the required build environment.

1. make sure that 'cmake', 'gcc', 'g++' and 'make' are in the path
2. cd to oryol/build
3. execute ./config_linux_debug.sh
4. cd to oryol/build/linux
5. execute 'make hello', this should compile the hello world sample
6. go to oryol/bin/linux, there should be a 'hello_debug' executable, running this should display 'Hello World!'

To compile the unit tests, run **config_linux_unittests.sh**, and then **make**, this will build everything and then run the unit tests automatically.


### emscripten ###

Cross-compilation to emscripten is currently only supported on OSX and Linux.

1. install the emscripten SDK on the same directory level as the Oryol SDK and name it 'emscripten' (e.g. if the Oryol SDK is under ~/oryol, place the emscripten SDK under ~/emscripten), to use another emscripten SDK location, set an EMSCRIPTEN_ROOT_PATH environment variable (see oryol/cmake/emscripten.toolchain.cmake for details)
2. cd to oryol/build
3. execute ./config_emsc_debug_make.sh
4. cd to oryol/build/emsc
5. execute 'make hello'
6. cd to oryol/bin/emsc
7. execute 'python -m SimpleHTTPServer', this runs a HTTP server at address '0.0.0.0:8000'
8. in a browser, navigate to 'http://0.0.0.0:8000/hello_debug.html'
9. you should see a web page with text field (may need to scroll down) and a "Hello World!" inside the text field

### Windows 32-bit ###

TODO

### Windows 64-bit ###

TODO

### iOS ###

TODO
