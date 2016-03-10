# _Oryol_
 
Experimental C++11 multi-platform 3D engine.

|Platform|Build Status|
|--------|------|
|OSX + Linux (OpenGL)|[![Build Status](https://travis-ci.org/floooh/oryol.svg?branch=master)](https://travis-ci.org/floooh/oryol)|
|Windows (OpenGL + D3D11)|[![Build status](https://ci.appveyor.com/api/projects/status/hn5sup2y532h64jg/branch/master?svg=true)](https://ci.appveyor.com/project/floooh/oryol/branch/master)|

## _Live Demos_

The Oryol samples page:

http://floooh.github.io/oryol/

A KC85 emulator using Oryol as app-wrapper:

http://floooh.github.io/virtualkc/

## _Build_

[How to Build](doc/BUILD.md)

## _Public Service Announcements_

- **10-Mar-2016**: Some API changes in the Gfx module which break existing 
code have been committed in the last few days, [here's an overview of what
has changed](http://floooh.github.io/2016/03/10/oryol-gfx-changes.html)

- **17-Oct-2015**: The Metal renderer is now also running on iOS (minimum
  supported version is iOS9). **Note** that you need an actual Metal-capable
  iOS device, it's not possible to test this on the iOS simulator! Give
  it a whirl with:

```bash
> ./fips set config metal-ios-xcode-debug
> ./fips gen
> ./fips open
```

- **08-Oct-2015**: You can now build the Metal version with the final OSX 10.11
  and Xcode7 versions (previously Xcode7-beta was needed):

```bash
# build and run on command line:
> ./fips set config metal-osx-xcode-release
> ./fips build
> ./fips run ImGuiDemo
# or build and debug in Xcode7:
> ./fips set config metal-osx-xcode-debug
> ./fips gen
> ./fips open
```

- **04-Oct-2015**: The D3D12 renderer backend is feature-complete and has been
  merged back into master! There are a number of Gfx API changes, until proper
  documentation is available look at the Oryol standalone sample app for the
  required code changes:
  https://github.com/floooh/oryol-test-app/commit/f14f46b1bdcf8bd9acb445bef10219916f700285.
  To test:

```bash
# make sure you have Win10, VS2015 and especially cmake-3.4 installed!
> fips set config d3d12-win64-vs2015-release
> fips build
> fips run ImGuiDemo
# or to compile and debug in Visual Studio:
> fips set config d3d12-win64-vs2015-debug
> fips gen
> fips open
```

- **24-Jun-2015**: the D3D11 renderer should now be on feature parity with the
  GL renderer, only some small optimizations are missing. To give it a whirl
  (all samples should work, except the NanoVG demo):

```bash
> fips set config d3d11-win64-vs2015-release
> fips build
> fips run SimpleRenderTarget
```
 There are also fips build-configs for VS2013, look in the fips-configs directory.


## _Read:_

* [Design Manifesto](doc/DESIGN-MANIFESTO.md)
* [10,000ft View](doc/OVERVIEW.md)
* [Core Module](code/Modules/Core/README.md)
* [Gfx Module](code/Modules/Gfx/README.md)

## Try

A simple standalone app using Oryol: https://github.com/floooh/oryol-test-app

## _Videos_ 

Please note that these videos use older versions of the Gfx module, details
have changed (and will continue to change at least until the Vulkan and DX12
renderer backends have been implemented).

- Coding a triangle from scratch [OSX](http://www.youtube.com/watch?v=B5R0uE5IMZs), [Windows](http://www.youtube.com/watch?v=fcmOhvVd80o)
- [Building a standalone app](https://www.youtube.com/watch?v=z8nwrGh2Zsc)
- [Compiling and debugging in QtCreator and CLion](https://www.youtube.com/watch?v=Sp5TywYeNzE)

#### Enjoy! ####




