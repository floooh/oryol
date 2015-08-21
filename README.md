# _Oryol_
 
Experimental C++11 multi-platform 3D engine.

|Platform|Build Status|
|--------|------|
|OSX + Linux (OpenGL)|[![Build Status](https://travis-ci.org/floooh/oryol.svg?branch=master)](https://travis-ci.org/floooh/oryol)|
|Windows (OpenGL + D3D11)|[![Build status](https://ci.appveyor.com/api/projects/status/hn5sup2y532h64jg/branch/master?svg=true)](https://ci.appveyor.com/project/floooh/oryol/branch/master)|

## _Live Demos_

http://floooh.github.io/oryol/

## _Build_

[How to Build](doc/BUILD.md)

## _Public Service Announcements_

- **21-Aug-2015**: As expected, the latest OSX 10.11 beta breaks the Metal port again, yay :D

- **17-Aug-2015**: The imgui wrapper has been updated for imgui 1.44, which 
requires an update to the fips-imgui dependency. Either delete the fips-imgui
directory and do a './fips fetch', or do a manual 'git pull && git submodule update'
in the fips-imgui directory to get the latest version

- **08-Aug-2015**: the Metal renderer has been fixed for the latest OSX 10.11 and Xcode7 beta, expect it to be broken again when the next betas come in ;)

- **27-Jul-2015**: the new OSX Metal renderer is feature-complete. To give it a
try you need the latest OSX10.11 and Xcode7 betas, and then:

```bash
> ./fips set config metal-osx-xcode-debug
> ./fips build
> ./fips run ImGuiDemo
```
...or use './fips open' to compile and debug in Xcode.

There is no Metal support for iOS yet (coming 'Really Soon Now').


- **24-Jun-2015**: the D3D11 renderer should now be on feature parity with the GL renderer, only some small optimizations are missing. To give it a whirl (all samples should work, except the NanoVG demo):

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




