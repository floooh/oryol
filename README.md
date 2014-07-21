oryol
=====

Experimental C++11 multi-platform 3D engine.

Live sample demos are here: http://floooh.github.io/oryol/

[![Build Status](https://travis-ci.org/floooh/oryol.svg?branch=master)](https://travis-ci.org/floooh/oryol)

### Cloning

Use **'git clone --recursive https://github.com/floooh/oryol.git oryol'** to clone the repo, so that you're getting any submodules.

If you have cloned without the submodules, use **'git submodule update --init --recursive'** to fetch the submodules.

### Warning

The Module APIs are currently extremely in flux, so don't use Oryol yet for anything serious!

### Goals

1. github-centric distributed development model, this is only an idea at the moment:
  - the engine is made of code modules, which are independent github projects
  - code modules compile to static link libraries
  - the 'main project' only has build system scripts, no actual source code
  - modules have a manifest file which defines dependencies to other modules
  - an Oryol project also has a manifest which lists the modules it needs
  - some build system magic gathers all the required module files from several github projects, and automagically creates the build files for compiling and linking the whole project

2. multi-platform build system:
  - at the core are cmake scripts with a lot of custom macros to simplify the definition of **libs** (3rd party code compiled into static link libs, **modules** (Oryol code modules with dependencies to other modules, compiled into static link libs), and **apps** (actual executables)
  - above cmake there's a python helper script which manages different build configurations (host platforms and their native IDEs / build tools, cross-compiling, release vs. debug etc...)

3. unit testing integrated into the development process:
  - Oryol integrates UnitTest++ into the build process, and writing unit tests is a seamless part of the development process

4. multi-platform with focus on web / mobile:
  - make it extremely easy to support new POSIX+OpenGL-style target platforms
  - make it possible to support non-POSIX / non-OpenGL-style target platfomrs
  - feature base-line is OpenGLES2 + some common GL extensions
  - support building on Linux, Mac, Windows (cmake makes this easy)

5. keep the code base lean and mean:
  - only implement what's necessary for a client-side game engine
  - only pull in external libs if they don't bloat the code base
  - client download size is important for web platforms

6. be web-friendly:
  - don't require pthread-style multi-threading, but support it when it's there
  - all data-loading happens asynchronously
  - use the HTTP philosophy for the IO system design instead of open/read/close

### Architecture:

- single main + render thread
- frame-callback based, don't own the game loop
- try to move compute tasks to GPU (e.g. transform flattening, animation evaluation, particle systems, ...)
- parallel-task system for expensive CPU work (also abstracts pthread-style and worker-style multithreading)

#### Core:
  - memory management, pool allocators
  - various string classes (immutable, UTF-8 vs. Wide, string atoms, ...)
  - various custom container classes (ring buffer, double-ended arrays, ...)
  - object-model: smart pointers and ref-counting
  - logging
  - some lock-less threading stuff that is not provided by std::atomic or std::thread
  - don't use C++ excpetions and rtti
  - custom assert with pretty-printed function signature, call stack and human-readable message

#### IO:
  - asynchronous file loading
  - URLs, Amiga-style Assigns, Streams, StreamReaders, StreamWriters
  - pluggable filesystems

### Messaging:
  - XML "message definition files" converted to C++ source/header files during build
  - message serializing to/from POD
  - asynchronous and synchronous message handling
  - threaded and non-threaded message handling

### HTTP:
  - simple HTTP client wrapper (use platform-specific APIs, or CURL as fallback)
  - simple HTTP server (only on platforms which support this)
  - HTTP pluggable-file-system
  - make it easy to handle JSON + XML formatted data

### Math:
  - use GLM (http://glm.g-truc.net/0.9.5/index.html) as math lib

### Timing:
  - use a thin wrapper around C++11's std::chrono if supported by the platform/compiler
  - fall back to platform-specific code if necessary

### Lowlevel Rendering:
  - low-level API wrapper implemented on top of OpenGL (ES2)
  - base feature set is OpenGL ES2 plus some common extensions
  - completely asynchronous resource loading
  - Twiggy-style medium-level rendering API (http://flohofwoe.blogspot.de/2012/08/twiggy.html)
  - shaders are "source code" (integrated into compile process, and linked into the executable)
  - GLFW as window system glue on supported platforms 

### Highlevel Rendering:
  [todo]

### Asset Files:
  - pluggable asset loaders
  - hopefully support glTF (https://github.com/KhronosGroup/glTF)
  - alternatively super-compact custom file format
  - drahtgitter python package for generating and converting assets: https://github.com/floooh/drahtgitter
  - use GLI for texture loading (http://www.g-truc.net/project-0024.html#menu)
