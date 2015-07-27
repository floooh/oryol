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

- **27-Jul-2015**: the new OSX Metal renderer is feature-complete. To give it a try you need the latest OSX10.11 and Xcode7 betas, and then:

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

## _Videos_

- Coding a triangle from scratch [OSX](http://www.youtube.com/watch?v=B5R0uE5IMZs), [Windows](http://www.youtube.com/watch?v=fcmOhvVd80o)
- [Building a standalone app](https://www.youtube.com/watch?v=z8nwrGh2Zsc)
- [Compiling and debugging in QtCreator and CLion](https://www.youtube.com/watch?v=Sp5TywYeNzE)

#### Enjoy! ####




