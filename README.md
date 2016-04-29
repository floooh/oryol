# _Oryol_
 
A small, portable and extensible C++ 3D coding framework:

- simple [Orthodox C++](https://gist.github.com/bkaradzic/2e39896bc7d8c34e042b) coding style and APIs
- extensible through external code modules living in git repositories
- runs on OSX, Linux, Windows, iOS, Android, emscripten, PNaCl,
Raspberry Pi from the same C++ source
- renders through GL, GLES2, WebGL, Metal, D3D11, D3D12 and
soon-ish Vulkan from the same C++ and shader source
- produces small executables (e.g. emscripten WebGL demos starting
at around 100 Kbytes)
- load data asynchronously from web or disc 

### Build Status

|Platform|Build Status|
|--------|------|
|OSX + Linux (OpenGL)|[![Build Status](https://travis-ci.org/floooh/oryol.svg?branch=master)](https://travis-ci.org/floooh/oryol)|
|Windows (OpenGL + D3D11)|[![Build status](https://ci.appveyor.com/api/projects/status/hn5sup2y532h64jg/branch/master?svg=true)](https://ci.appveyor.com/project/floooh/oryol/branch/master)|

### Getting Started:

* [How to Build](doc/BUILD.md)
* [What's New](doc/NEWS.md)
* [Design Manifesto](doc/DESIGN-MANIFESTO.md)
* [10,000ft View](doc/OVERVIEW.md)
* [Core Module](code/Modules/Core/README.md)
* [IO Module](code/Modules/IO/README.md)
* [Gfx Module](code/Modules/Gfx/README.md)

### Live Demos

- Oryol Core Samples: http://floooh.github.io/oryol/
- Oryol Extension Samples: http://floooh.github.io/oryol-samples/
- KC85 emulator: http://floooh.github.io/virtualkc/

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




