# Oryol
 
A small, portable and extensible 3D coding framework written in C++:

- simple [Orthodox C++](https://gist.github.com/bkaradzic/2e39896bc7d8c34e042b) coding style and APIs
- extensible through external code modules living in git repositories
- runs on OSX, Linux (incl RaspberryPi), Windows, iOS, Android, emscripten, from the same C++ source
- renders through GL, GLES2, WebGL, Metal, D3D11 from same shader source
- produces small executables (e.g. emscripten WebGL demos starting at around 100 Kbytes)
- async data loading from web or disc 

### Build Status:

|Platform|Build Status|
|--------|------|
|OSX + Linux (OpenGL)|[![Build Status](https://travis-ci.org/floooh/oryol.svg?branch=master)](https://travis-ci.org/floooh/oryol)|
|Windows (OpenGL + D3D11)|[![Build status](https://ci.appveyor.com/api/projects/status/hn5sup2y532h64jg/branch/master?svg=true)](https://ci.appveyor.com/project/floooh/oryol/branch/master)|

### Live Demos:

- Oryol Core Samples: http://floooh.github.io/oryol/
- Oryol Extension Samples: http://floooh.github.io/oryol-samples/
- KC85 emulator: http://floooh.github.io/virtualkc/

### How to Build (Quick'n'Dirty):

You need: cmake, python and your platform's default C/C++ development environment.

```bash
> mkdir projects
> cd projects
> git clone --depth 5 https://github.com/floooh/oryol
> cd oryol
> ./fips build
> ./fips run Triangle
```

In case of problems or for more detailed build info (e.g. how to work
with IDEs) see here: [How to Build](doc/BUILD.md)

### Getting Started:

* [What's New](doc/NEWS.md) (updated: 22-May-2017)
* [How to Build](doc/BUILD.md)
* [Design Manifesto](doc/DESIGN-MANIFESTO.md)
* [10,000ft View](doc/OVERVIEW.md)
* [Core Module](code/Modules/Core/README.md)
* [IO Module](code/Modules/IO/README.md)
* [Input Module](code/Modules/Input/README.md)
* [Gfx Module](code/Modules/Gfx/README.md)
* [Resource Module](code/Modules/Resource/README.md)
* [HttpFS Module](code/Modules/HttpFS/README.md)
* [LocalFS Module](code/Modules/LocalFS/README.md)

### Useful Blog Posts:

* [System Design Philosophy](http://floooh.github.io/2017/07/01/oryol-system-design.html)
* [SPIRV-based Shader Pipeline](http://floooh.github.io/2017/05/15/oryol-spirv.html)
* [WebGL2/RenderPass Merge](http://floooh.github.io/2017/04/04/oryol-webgl2-merge.html)
* [About Webpage Integration (asm.js/wasm)](http://floooh.github.io/2017/02/22/emsc-html.html)
* [Slimmer asm.js/wasm Applications](http://floooh.github.io/2016/08/27/asmjs-diet.html)
* [2016 Spring Cleaning](http://floooh.github.io/2016/04/26/oryol-spring-cleaning.html)
* [Tour of 3rd-party Code (2016)](http://floooh.github.io/2016/04/09/oryol-3rd-party-code.html)
* [Gfx Module Changes (2016)](http://floooh.github.io/2016/03/10/oryol-gfx-changes.html)
* [Tour of the Metal Renderer](http://floooh.github.io/2016/01/15/oryol-metal-tour.html)

### Extension Modules:

- Skeletal Animation System: https://github.com/floooh/oryol-animation
- Dear Imgui integration: https://github.com/floooh/oryol-imgui
- Nuklear UI integration: https://github.com/floooh/oryol-nuklear
- TurboBadger UI integration: https://github.com/floooh/oryol-tbui
- OpenAL based sound module: https://github.com/floooh/oryol-sound
- SoLoud portable audio library: https://github.com/floooh/fips-soloud

See the [Oryol Extension Sample](http://floooh.github.io/oryol-samples/) webpage for more interesting 3rd-party library integrations.

### Tools etc.:

- Oryol Asset Tools (WIP): https://github.com/floooh/oryol-tools
- Oryol Fileformat Definitions: https://github.com/floooh/oryol-fileformats

### Standalone App Demo:

A simple standalone app using Oryol: https://github.com/floooh/oryol-test-app

### Videos 

Please note that these videos use older versions of the Gfx module, details
have changed (and will continue to change at least until the Vulkan and DX12
renderer backends have been implemented).

- Coding a triangle from scratch [OSX](http://www.youtube.com/watch?v=B5R0uE5IMZs), [Windows](http://www.youtube.com/watch?v=fcmOhvVd80o)
- [Building a standalone app](https://www.youtube.com/watch?v=z8nwrGh2Zsc)
- [Compiling and debugging in QtCreator and CLion](https://www.youtube.com/watch?v=Sp5TywYeNzE)

#### Enjoy! ####




