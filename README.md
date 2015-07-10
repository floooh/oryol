# _Oryol_
 
Experimental C++11 multi-platform 3D engine.

[![Build Status](https://travis-ci.org/floooh/oryol.svg?branch=master)](https://travis-ci.org/floooh/oryol)

## _Live Demos_

http://floooh.github.io/oryol/

## _Public Service Announcements_

- **10-Jul-2015**: (TL;DR: don't install the OSX 10.11 beta yet) Careful if you want to install the OSX 10.11 beta, brew's python 2.7.x installation is currently broken on OSX10.11 which breaks some Oryol code generator scripts (everything that requires the python zlib module), the standard OSX python installation can be used as a workaround, but this doesn't provide a python2 command, which is required by the emscripten build scripts 

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

## _Clone_

```bash
git clone https://github.com/floooh/oryol.git oryol
```

## _Build_

[How to Build](doc/BUILD.md)

## _Videos_

- Coding a triangle from scratch [OSX](http://www.youtube.com/watch?v=B5R0uE5IMZs), [Windows](http://www.youtube.com/watch?v=fcmOhvVd80o)
- [Building a standalone app](https://www.youtube.com/watch?v=z8nwrGh2Zsc)
- [Compiling and debugging in QtCreator and CLion](https://www.youtube.com/watch?v=Sp5TywYeNzE)

#### Enjoy! ####




