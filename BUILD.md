Oryol uses a python script ('oryol' in the project root directory) and
CMake to manage the build configuration process for the supported host and target platforms.

## Prerequisites

### Host and Target Platforms

Oryol supports the following host platforms for building:

- OSX
- Linux
- Windows (planned)

...and the following target platforms:

- OSX
- Linux
- Windows (planned)
- iOS (planned)
- Android (planned)
- emscripten 
- PNaCl (planned, no C++11 support yet in the SDK?)

Not all target platforms can be built on all host platforms!

### Tools

The following tools are required on all host platforms:

- python (2.7.x, 3.x is not currently supported)
- cmake (at least version 2.8.11)

#### OSX Tools

- homebrew is recommended as package manager (http://brew.sh/)
- Xcode 5 and the Xcode command line tools are required

#### Linux Tools

- gcc4.8 is required 
- the ninja build tool is recommended, but not required

#### Windows

TODO

## Building Oryol

If the above prerequisites are met you should be able to simply run:

```
./oryol build
```

from the Oryol root directory to run a default command line build for your operating system.

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
  emscripten-make-release
  emscripten-ninja-debug
  emscripten-ninja-release
  linux-make-debug
  linux-make-release
  linux-make-unittests
  linux-ninja-debug
  linux-ninja-release
  linux-ninja-unittests
  osx-xcode-debug
  osx-xcode-release
  osx-xcode-unittests
  pnacl-make-debug
  pnacl-make-release
  pnacl-ninja-debug
  pnacl-ninja-release
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
