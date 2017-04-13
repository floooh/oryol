# Building Oryol

Oryol uses the fips build system, which is actually the former
Oryol build system moved out into its own project. All build tasks
are controlled through a python script 'fips' in the Oryol root
directory.

>NOTE: on OSX and Linux, invoke the 'fips' script with './fips', on Windows, 
simply run 'fips'

## TL;DR: how to build and run the samples

### You need:

- python 2.7.x
- cmake 2.8.11 or better
- a working C/C++ development environment for your platform

### Prerequisites:

First make sure all required 3rd party tools are present:

```bash
> cd oryol
> ./fips diag tools
...
```

Install any tools that are not found.

### Building and running the samples:

```bash
> cd oryol
> ./fips build
...
> ./fips run Triangle
> _
```

### Using IDEs

To open Oryol in Visual Studio or Xcode:

```bash
> cd oryol
> ./fips gen
> ./fips open
```

Besides VStudio and Xcode, other IDEs that can load cmake projects
are also supported (like QtCreator or CLion), but not currently
through './fips open'.

### Read the docs

More detailed information about fips can be found here:

http://floooh.github.com/fips

### Building for 3D-APIs other than OpenGL:

Oryol has support for non-OpenGL rendering backends on some platforms which
can be easily selected via fips build configs:

##### D3D11:

Building with D3D11 support requires at least Windows7 and a recent 
Visual Studio version (VS2013 or VS2015). Select any of the following build 
configs:

- d3d11-win64-vstudio-debug
- d3d11-win64-vstudio-release

For example:

```bash
# to build and run on command line:
> fips set config d3d11-win64-vstudio-release
> fips gen
> fips run Triangle
# to build and debug in Visual Studio:
> fips set config d3d11-win64-vstudio-debug
> fips gen
> fips open
```

##### Metal (OSX and iOS):

Building with Metal support requires OSX 10.11 and Xcode7. For iOS, a
Metal-capable iOS device is needed running at least iOS9.

Use any of the following build configs to build for OSX:

- metal-osx-xcode-debug
- metal-osx-xcode-release
- metal-osx-make-debug
- metal-osx-make-release

...and those build configs for iOS9:

- metal-ios-xcode-debug
- metal-ios-xcode-release

### Cross-compiling

#### emscripten:

To build and run the samples for emscripten, first setup the emscripten SDK:

> NOTE: GNU make is not supported on Windows, please use the 'emsc-ninja-release'
> config instead, which uses a pre-compiled ninja build tool included in the
> fips build system distribution

```bash
> cd oryol
> ./fips setup emscripten
...
> ./fips build emsc-make-release
...
> ./fips run Triangle emsc-make-release
> _
```

Running the Triangle sample will start a local web server and launch the
sample in the default web browser. It may be necessary to manually 
refresh the browser page (F5).

#### iOS

To compile and debug Oryol directly in Xcode (provided iOS development
has been setup in Xcode):

```bash
> ./fips gen ios-xcode-debug
...
> ./fips open ios-xcode-debug
> _
```

#### Android:

> NOTE: on Windows, please use the build config 'android-ninja-release' instead
> of 'android-make-release', since GNU make is not supported on Windows

```bash
> cd oryol
> ./fips setup android
...
> ./fips build android-make-release
> _
```

See the fips documentation for more details about Android development
with fips.


### Building the Web Samples HTML page

To build the complete samples web page (provided you have a working emscripten installation):

```
> cd oryol
> ./fips webpage build
...
> ./fips webpage serve
...
```

