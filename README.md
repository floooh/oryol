# _Oryol_
 
Experimental C++11 multi-platform 3D engine.

|Platform|Build Status|
|--------|------|
|OSX + Linux (OpenGL)|[![Build Status](https://travis-ci.org/floooh/oryol.svg?branch=master)](https://travis-ci.org/floooh/oryol)|
|Windows (OpenGL + D3D11)|[![Build status](https://ci.appveyor.com/api/projects/status/hn5sup2y532h64jg/branch/master?svg=true)](https://ci.appveyor.com/project/floooh/oryol/branch/master)|

## _Live Demos_

- the Oryol **samples page**: http://floooh.github.io/oryol/
- **KC85 emulator** using Oryol as app-wrapper: http://floooh.github.io/virtualkc/
- **voxel demo** built on top of stb\_voxel\_render.h: http://floooh.github.io/voxel-test/ ([source](https://github.com/floooh/voxel-test))

## _Build_

[How to Build](doc/BUILD.md)

## _Public Service Announcements_

- **08-Apr-2016**: I have removed the 'git lfs' requirement again, since
it doesn't work with github-pages, and all the big files are in the
github-pages branch, so in the end it didn't make sense 
~~the oryol repo now uses 'git lfs' for some files under
data, and the gh-pages branch for the samples webpage, if you haven't
please install git lfs support (https://git-lfs.github.com/)~~

- **07-Apr-2016**: I have added a new sample which demonstrates how
to integrate the [SoLoud audio lib](http://sol.gfxile.net/soloud/), here's
the web demo: [SoloudTedSid](http://floooh.github.io/oryol/asmjs/SoloudTedSid.html)
 
- **10-Mar-2016**: Some API changes in the Gfx module which break existing 
code have been committed in the last few days, [here's an overview of what
has changed](http://floooh.github.io/2016/03/10/oryol-gfx-changes.html)


## _Read:_

* [Design Manifesto](doc/DESIGN-MANIFESTO.md)
* [10,000ft View](doc/OVERVIEW.md)
* [Core Module](code/Modules/Core/README.md)
* [IO Module](code/Modules/IO/README.md)
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




