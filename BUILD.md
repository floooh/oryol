Oryol uses a python script ('oryol' in the project root directory) and
CMake to manage build configurations for the supported host and target platforms.

## TL;DR: how to build and run the samples

### You need: ###
- python 2.7.x
- cmake 2.8.11 or better
- for **OSX:** Xcode 5.x
- for **Windows:** VStudio 2013 (Express should work fine)
- for **emscripten:** 'node' must be in the path, and a JRE must be installed
- for **Android:** the 'ant' Java build tool must be in the path

### OSX ###
To build and run the Triangle sample on OSX:

```
> cd ~/oryol
> ./oryol build osx-xcode-debug 
[this should run cmake and compile the samples]
> cd bin/osx
> ls
Clear.app Triangle.app ...
> open Triangle.app
```

### Windows ###
To build and run the (32-bit) Triangle sample on Windows:

```
> cd oryol
> oryol build win32-vstudio-debug
[this should run cmake and compile the samples]
> cd bin\win32
> dir *.exe
...
07.04.2014  19:31         2.651.648 Triangle_debug.exe
...
> Triangle_debug.exe
```

To build the 64-bit samples:

```
> cd oryol
> oryol build win64-vstudio-debug
[this should run cmake and compile the samples]
> cd bin\win64
> dir *.exe
...
07.04.2014  19:48         3.194.368 Triangle_debug.exe
...
> Triangle_debug.exe
```

### emscripten ###
To build and run the Triangle sample for emscripten (NOTE: you also 
need node.js and Java runtime installation):

```
> cd ~/oryol
> ./oryol setup emscripten
> ./oryol build emscripten-make-release
[this should run cmake and compile the samples]
> cd bin/emsc
> ls
Clear.html Shapes.html Triangle.html ...
> python -m SimpleHTTPServer
Serving HTTP on 0.0.0.0 port 8000 ...
```
Now start Chrome or Firefox and navigate to **http://0.0.0.0:8000**

### Android ###
To setup a local Android development environment, build and deploy the
Triangle sample to your Android device (with unlocked development mode):

```
> brew install ant
[...or use the package manager of your choice]
> cd ~/oryol
> ./oryol setup android
[...this downloads the required SDKs...]
> ./oryol select android-make-debug
> ./oryol make Triangle
> ./oryol deploy Triangle
```
If everything has worked, there should be a new Triangle app installed,
tap to run.

### Google Native Client ###
To setup the NaCl SDK, and build the Triangle sample on OSX or Linux:

**Linux Note:** on a 64-bit Linux you need to install 32-bit runtime libs first:
* sudo apt-get install libc6:i386
* sudo apt-get install libstdc++6:i386

```
> cd ~/oryol
~ ./oryol setup nacl
[...this downloads the NaCl SDK...]
> ./oryol select pnacl-make-release
> ./oryol make Triangle
> cd bin/pnacl
> python -m SimpleHTTPServer
```

Now open a recent version of Google Chrome and navigate to http://localhost:8000/Triangle_pnacl.html.

### Building the Web Samples HTML page ###
To build the complete samples web page (provided you have a working emscripten SDK installation), simply run:

```
> cd ~/oryol
> ./oryol webpage
[this should run cmake, compile the samples, and build an index.html page]
> cd build/webpage
> python -m SimpleHTTPServer
Serving HTTP on 0.0.0.0 port 8000 ...
```


## Prerequisites

### Host and Target Platforms

Oryol supports the following host platforms for building:

- OSX
- Linux
- Windows

...and the following target platforms:

- OSX
- Linux
- Windows 32-bit and 64-bit
- iOS
- Android
- emscripten 
- PNaCl

There are restrictions for cross-compiling:

- host OS OSX can build for: OSX, emscripten, PNaCl, iOS, Android
- host OS Linux can build for: Linux, emscripten, PNaCl, Android
- host OS Windows can build for:  Windows 32-bit and Windows 64-bit

### Tools

The following tools are required on all host platforms:

- python (2.7.x, 3.x is not currently supported)
- cmake (at least version 2.8.11)
- cmake-gui and/or ccmake (on some platforms these are installed with the cmake package)

Make sure all these tools are in the path!

#### OSX Tools

- Xcode 5 and the Xcode command line tools are required

#### Linux Tools

- gcc4.8 is required 
- the ninja build tool is recommended, but not required

#### Windows

- get **Visual Studio 2013 Express for Windows Desktop** (not 'for Windows', that's for Windows8 metro apps only!)

#### Android

- make sure 'ant' is in the path (e.g. on OSX: 'brew install ant')

#### Emscripten

- make sure 'node' is in the path (e.g. on OSX: 'brew install node')
- make sure a JRE is installed: 'java -version'

## Using The oryol Build System Wrapper Script

#### Check if the oryol script is working ####

First, try to run the **oryol** python script without args from the command line:

```
On OSX/Linux:
> ./oryol
On Windows:
> oryol
```

This should print a summary of command line args and what they do.

The oryol script simplifies working with the many combinations of target platforms, build tools and build configurations by adding an abstraction layer above cmake. Usually instead of calling cmake directly, the oryol script is invoked with command line arguments.

#### Build Configs ####

A build config describes how Oryol build files should be created for a specific target platform, build environment and compilation modes (like Release and Debug).

To see a list of currently supported build configs:

```
> ./oryol list configs
```

This should dump a list containing lines like this:

```
emscripten-make-debug
linux-ninja-release
osx-xcode-debug
win32-vstudio-debug
```

The config names are descriptive: the first part indicates the target platform (e.g. emscripten, linux, osx...), the second part the build tool or IDE (make, ninja, xcode or vstudio), and the third part the build config (here: debug or release).

Each host platform has a default config, which is one of these:

```
linux-make-debug
osx-xcode-debug
win32-vstudio-debug
```

#### The Active Config ####

To save typing when working with the oryol build script you can set an **active config**, which will then automatically be used when no specific configuration name is specified.

**To set the active configuration:**
```
> ./oryol select [config-name]
```

**To display the current active configuration:**
```
> ./oryol list selected
```

For instance on OSX:

```
> ./oryol select osx-xcode-debug
'osx-xcode-debug' selected as current config
> ./oryol list selected
selected config is: 'osx-xcode-debug'
```

To make the default configuration of the current host platform active, simply use select without a config name:

```
> ./oryol select
'osx-xcode-debug' selected as current config
> ./oryol list selected
selected config is: 'osx-xcode-debug'
```

#### Generating the Build Files ####

To generate the Makefiles, ninja-files or IDE-specific project files for the currently active config, use the
**update** argument:

```
> ./oryol update
```

This will dump a lot of stuff to the console, starting and ending like this:

```
selected config is 'osx-xcode-debug'
cmake: found
xcodebuild: found
cmake -G Xcode -DCMAKE_BUILD_TYPE=Debug -DORYOL_UNITTESTS=OFF -DORYOL_EXCEPTIONS=OFF ../..
-- The C compiler identification is Clang 5.1.0
...
...
-- Configuring done
-- Generating done
-- Build files have been written to: /Users/floh/projects/oryol/build/osx-xcode-debug
```

Note the last line which says where the build files have been written to. Since Xcode project files have been generated, you can now open Xcode, and open the project **oryol.xcodeproj** file from this directory.

You can also specify a configuration name after update, to generate build files for a specific configuration:

```
> ./oryol update emscripten-ninja-debug
```

Or you can just give part of a configuration name, for instance to create the build files for all emscripten-configs, you could run:

```
> ./oryol update emscripten
```

Or if you only want the ninja files, but not make:

```
> ./oryol update emscripten-ninja
```

Finally, you can also update **all** configs which are supported on your host platform:

```
> ./oryol update all
```

#### Building from the Command Line ####

You can simply build all targets in the active config on the command line (also for IDE configs like Xcode or Visual Studio) like this:

```
> ./oryol build
```

To build a specific config, just append the config name:

```
> ./oryol build emscripten-make-release
```

The oryol script will do some behind the scene magic, for instance create the build files if no update was called beforehand, and try to activate parallel builds (usually over 3 jobs).

The resulting executables can be found in one of the following directories (depending on the target platform):

```
oryol/bin/win32
oryol/bin/win64
oryol/bin/osx
oryol/bin/linux
oryol/bin/emsc
oryol/bin/pnacl
oryol/bin/android
oryol/bin/ios
```

Note that configurations for the same target platform will currently overwrite each other's executable!

#### Building with IDEs ####

Use 'oryol open' to open the currently selected config in an IDE. Currently this only works for Xcode and VStudio projects:

```
> ./oryol open
selected config is 'osx-xcode-debug'
Opening 'osx-xcode-debug' in Xcode...
```

...or you can open a specific config by appending the config name:

```
> ./oryol open osx-xcode-unittests-debug
Opening 'osx-xcode-unittests-debug' in Xcode...
```

For other IDEs this is currently not supported. You must start the IDE manually and find the generated project file in the **build/[config-name]** subdirectory.

#### Building Specific Targets ####

Use the **make** arg if you only want to compile one executable, or generally want to evaluate a specific build target.
For instance to only build the InfiteSpheres sample in the active configuration:

```
> ./oryol make InfiniteSpheres
```

You can also append a specific config name:

```
> ./oryol make InfiniteSpheres emscripten-make-debug
```

#### Configuring Builds ####

The oryol script can call either the cmake-gui or ccmake configuration tools for fine-tuning the build configuration. To start configuring the active configuration, simply call:

```
> ./oryol config
```

This will first try to start ccmake, and if this is not available, cmake-gui. If none of those can be called, an error will be displayed.
``

#### Starting From Scratch ####

You can delete the build directory of one or more configuration with the **clean** arg:

```
> ./oryol clean
> ./oryol clean [config-name]
> ./oryol clean all
```

This deletes all files generated by cmake and the build process, like IDE project files, make and ninja files, compiled object files and static libraries, etc...

## Android Cross-Compiling

**NOTE:** this only works on OSX at the moment!

Before Android development can start, setup the required SDKs. The ./oryol python script makes this easy:

```
> cd ~/oryol
> ./oryol setup android
```

This will download and prepare the Android SDK and the Android NDK, and setup a standalone GCC toolchain, all in the 'sdks' subdirectory. Your environment will not be altered (no environment variables or paths will be set).

You must install the 'ant' build tool though. On OSX I recommend the brew package manager (http://brew.sh/)

```
> brew install ant
==> Downloading http://www.apache.org/dyn/closer.cgi?path=ant/binaries/apache-ant-1.9.3-bin.tar.gz
Already downloaded: /Library/Caches/Homebrew/ant-1.9.3.tar.gz
/usr/local/Cellar/ant/1.9.3: 1592 files, 39M, built in 3 seconds
```

Next, make sure that the Development Mode on your Android device is unlocked (see here: http://developer.android.com/tools/index.html):

Here's the important part:
```
On Android 4.2 and higher, the Developer options screen is hidden by default. 
To make it available, go to Settings > About phone and tap Build number seven times. 
```

Connect the device to your computer via USB, and verify that it is visible with 
the 'adb' tool in the Android SDK:

```
> cd ~/oryol
> sdks/osx/android-sdk-macosx/platform-tools/adb devices
List of devices attached 
0xxxxxxx  device

```

And that's it! If everything is setup right, you can now select one of the Android build configurations,
compile a sample, and deploy it to your device:

```
> ./oryol select android-make-debug
> ./oryol make PackedNormals
> ./oryol deploy PackedNormals 
...
3275 KB/s (1726765 bytes in 0.514s)
	pkg: /data/local/tmp/PackedNormals-debug.apk
Success
```

The App Launcher (or whatever it is called) now has a new PackedNormals app!


## Cross-platform builds with Vagrant

NOTE: this probably doesn't work right now!

Oryol comes with a Vagrantfile which sets up a Linux VM with all required tools and SDKs to compile
native Linux builds and cross-compile to emscripten and PNaCl.

First you need to setup vagrant:

1. get VirtualBox for your host operating system (https://www.virtualbox.org/wiki/Downloads)
2. get Vagrant for your host OS (http://www.vagrantup.com/downloads.html)
3. install the vagrant-vbguest plugin: ```vagrant plugin install vagrant-vbguest```

Now you're ready to setup the VM with **vagrant up**

```
flohofwoe:oryol floh$ pwd
/Users/floh/oryol
flohofwoe:oryol floh$ cd vagrant
flohofwoe:vagrant floh$ vagrant up
```

This will download a Linux image and install all required tools and SDKs inside the VM, so this will take a while, go make a sandwich or something...

After everything's setup, connect to the VM:

```
flohofwoe:vagrant floh$ vagrant ssh
Welcome to Ubuntu 12.04 LTS (GNU/Linux 3.2.0-23-generic x86_64)

 * Documentation:  https://help.ubuntu.com/
Welcome to your Vagrant-built virtual machine.
Last login: Sat Feb  1 15:50:56 2014 from 10.0.2.2
vagrant@precise64:~$ cd oryol/
vagrant@precise64:~/oryol$ ls
bin  build  BUILD.md  cmake  CMakeLists.txt  code  configs  data  doc  IDEAS.md  LICENSE  oryol  README.md  vagrant
vagrant@precise64:~/oryol$ ./oryol build all
```

There's you (shared) oryol directory, ready to go!
