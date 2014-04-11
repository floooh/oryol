Oryol uses a python script ('oryol' in the project root directory) and
CMake to manage build configurations for the supported host and target platforms.

## TL;DR: how to build and run the samples

### You need: ###
- python 2.7.x
- cmake 2.8.11 or better
- for **OSX:** Xcode 5.x
- for **Windows:** VStudio 2013 (Express should work fine)
- for **emscripten:** a working emscripten fastcomp compile environment

### OSX ###
To build and run the Triangle sample on OSX:

```
> cd oryol
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
To build and run the Triangle sample for emscripten:

```
> cd oryol
> ./oryol build emscripten-make-release
[this should run cmake and compile the samples]
> cd bin/emsc
> ls
Clear.html Shapes.html Triangle.html ...
> python -m SimpleHTTPServer
Serving HTTP on 0.0.0.0 port 8000 ...
```

Now start Chrome or Firefox and navigate to **http://0.0.0.0:8000**

### Building the Web Samples HTML page ###
To build the complete samples web page (provided you have a working emscripten SDK installation), simply run:

```
> cd oryol
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
- iOS (planned)
- Android (planned)
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

Make sure python and cmake are in the path!

#### OSX Tools

- Xcode 5 and the Xcode command line tools are required

#### Linux Tools

- gcc4.8 is required 
- the ninja build tool is recommended, but not required

#### Windows

- get **Visual Studio 2013 Express for Windows Desktop** (not 'for Windows', that's for Windows8 metro apps only!)

## Building Oryol

If the above prerequisites are met you should be able to simply run:

```
./oryol build
```

from the Oryol root directory to run a default command line build for your operating system.

**Windows NOTES**: 
On Windows, an additional **oryol.cmd** batch file exists, which can be invoked simply with 
**oryol**.

After this finishes, open the generated VStudio solution file: ```build/win32-vstudio-debug/oryol.sln```!


After the build has finished, you should find a **hello_debug** executable under either
**oryol/bin/osx**, **oryol/bin/linux** or **oryol/bin/win32**. Running this
executable from the command line should print a **Hello World!**.

### Using the ./oryol Python script ###

The ./oryol script is basically just a front-end to manage different CMake build 
configurations. A build configuration has the following properties:

1. a unique name (e.g. 'osx-xcode-debug')
2. the target platform (e.g. 'osx', 'linux', 'emscripten')
3. the CMake generator to use, for instance whether to use 'make' or 'ninja' for building, whether to generate XCode or VisualStudio project files
4. the build-type (e.g. 'release', 'debug', 'unittests')

Additionally, any CMake properties can be added to the configuration (this is planned, not yet
implemented).


### Working with Configurations ###

To **list all configurations**, type 

```
> ./oryol list configs
```

This should print a list of the currently existing configurations, e.g.:

```
flohofwoe:oryol floh$ ./oryol list configs
CONFIGS:
  emscripten-make-debug
  ...
  linux-make-debug
  ...
  osx-xcode-debug
  ...
  pnacl-make-debug
  ...
  win32-vstudio-debug
```

You can **select the active configuration** which will be used if no specific config name 
is provided:

```
> ./oryol select osx-xcode-unittests
```

If you omit the configuration name, the default configuration name for your host platform
will be selected. 

**NOTE** that you cannot select a configuration which would be invalid for your platform, so trying
to run the above command on Linux or Windows would display an error.

To **generate build files** for the active configuration with:

```
> ./oryol update
```

Or to generate build files for a specific configuration:

```
> ./oryol update linux-ninja-release
```

You can now either run a command line build (see below), or open the generated project file in Xcode or
Visual Studio by looking in the **build/[config-name]** subdirectory for an **oryol.xcodeproj**
or **oryol.sln** file.

To **build the currently selected configuration** on the command line, run:

```
> ./oryol build
```

To build a specific configuration, run for instance:

```
> ./oryol build osx-xcode-release
```

For a **complete rebuild**, you can just as well use the **rebuild** command instead of **build**:

```
> ./oryol rebuild
```

To delete all generated build files for a config, run:

```
> ./oryol clean [config-name]
```

Finally, you can use the special config name **all** to invoke a command on all valid 
configurations for the host platform:

```
> ./oryol clean all
> ./oryol update all
> ./oryol build all
> ./oryol rebuild all
```

## Cross-platform builds with Vagrant

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
