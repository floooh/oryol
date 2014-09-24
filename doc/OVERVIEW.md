## 10,000ft View

### What's in the box

Let's get the bullet-point checklist out of the way first :)

- currently runs on iOS, Android, emscripten, PNaCl, Win32, Win64 (not WinRT), OSX and Linux
with some features lagging behind on some platforms
- builds on Linux, OSX, Windows with native IDE support (QtCreator, Xcode, VStudio)
- cmake based build system with a frontend python script
- GLSL shader coding integrated into IDE with Khronos reference compiler integrated 
and clickable error messages
- python/XML-driven C++ code generation integrated into IDE/build process
- written in a fairly high-level form of C++(11)
- asynchronous asset loading from web servers
- currently uses the following 3rd party libs: 
    - LLVM's UTF-8/wide-char conversion routines
    - libcurl
    - GLEW (probably will be replaced by flextGL)
    - glfw3
    - glm
    - UnitTest++
    - zlib

### Motivation

The question is: how small can a game engine be while still being useful and extensible?

'Small' means 'little complexity', 'few lines of code', 'small client executable size'.

An Oryol executable shouldn't contain any dead code, and everything should be linked statically
into a single executable and no DLLs (except the absolutely required system DLLs). 

Oryols features are implemented in code modules, which compile to static link libraries.
Module dependencies are defined in the build system files per module and resolved
to a list of linked libraries at link time. 

There's a clear module hierarchy (e.g. every module will depend on the Core module, 
but the Core module must never depend on another higher-level module). 

I think a minimal, portable game engine should at least provide the following 
low-level services:

- a unified way for app entry and running the game loop
- input (mouse, keyboard, touch, gamepad)
- 3D rendering
- 3D audio (at least simple 3D-positioned sound effects)
- asset loading and simple tools to convert asset files to engine file formats
- a lowlevel animation system
- time measurement
- logging

These low-level modules should be as generic as possible and for instance not
enforce a specific rendering technique.

Above these low-level modules are more specialized, higher-level modules, for 
instance:

- a 3D model rendering system which can efficiently render multi-material models
- one or more realtime lighting modules implementing different lighting techniques
- a higher-level animation 
- a 3D character model rendering and animation system

Above this there _could_ be a entity-component system which integrates all of this
with an editor UI. But specifically this is a non-goal for Oryol. Big engines
like Unreal or Unity will always this type of integrated environment much better.

One feature I'd like to explore more is the feasability of games which don't have
traditional asset loading at all. Have very few assets and link them directly 
into the executable, or generate assets on the fly.

### Build System

A multiplatform build system is a complex beast. It should be able to 
support the native compilers of a host platform (for instance Visual Studio
on Windows, instead of mingw/gcc), it must support cross-compiling and it
should support IDEs. I already had experience with cmake which fits these
requirements, even though it is itself a complex beast.

Managing dozens of build systems alone is not very comfortable, so (inspired by
emscripten's emcc python script) a frontend python script 'oryol' exists, which
calls cmake and over time a lot of comfort features where added (like automatically 
setting up the Android and emscripten SDKs, deploying a target to an mobile devices,
or opening the IDE for a build configuration).

CMakeLists.txt files use a very simplified custom syntax to define
the type of target (module, app or extlib), the source code directory (with
the ability to filter sources by target platform).

### Modules, ExtLibs and Apps

An Oryol Module is a group of related source code under a directory and compiled
into a static link library. Modules define dependencies to other modules and ExtLibs,
which are then automatically resolved when linking Apps.

An ExtLib is a special lowlevel type of module for 3rd party libs (like libcurl, GLFW, glm, ...).
ExtLibs can be pure C libs, and can define their own compiler settings. Extlibs should
not depend on each other and should not depend on regular modules. Extlibs can be header-only
with a precompiled library under oryol/libs.

An App is an executable linked against modules and extlibs. Module dependencies 
will be automatically resolved (for instance, the Gfx module depends on the Core
module, in the App's CMakeLists.txt file, only the Gfx module must be listed
as dependency, the Core module will be automatically pulled in because the Gfx 
module depends on it).

### C++ Features

Like possibly every C++ code I have a love/hate relationship with the language.
I wrote a blogpost about what I think is "sane C++" a while ago:

http://flohofwoe.blogspot.de/2013/06/sane-c.html

When I started with Oryol I tried to make a clean sweap and tried to 
look at C++11 and the new STL again with an open mind. I wrote benchmarks, contemplated
about the look and feel of the code, tried out a lot of things and formed a new 
view - which isn't much different from my old view unfortunately.

I still think exceptions are useless: I'm a bug fan of the 'let it crash'
philosophy. Of course an app shouldn't get into an invalid state, but when
it does, most of the time it doesn't even make sense to try to recover because
it's not clear what might be broken. It's best to crash and dump as much 
information for post-mortem debugging as possible. 

An exception which isn't bad enough to terminate an app immediately is 
hardly an exception, but a really expensive status code.

For the 'crash and dump information', exceptions are surposingly useless.
What I need is a callstack from the location where the exception was thrown,
and some local information of what went wrong and where, like, for instance
a print-out of the failed condition, a filename and a line count. On GCC one 
can hijack __cxa_throw and generate a stack trace there, but why implement
such hacks on top of a complex system which is useless anyway, when asserts
do a much better job with infinitely less complexity?

It is possible to compile Oryol with exceptions enabled (this is necessary for
UnitTest++), but it is not recommended, and Oryol itself will always be 
exception free.

I still think that the std containers are useless. First they communicate failure
with exceptions. I know that something went wrong, but only very little 
what went wrong since I'm not getting a callstack from where the exception thrown. 
I think that the std container classes are plagued by over-engineering and
the need for backward compatibility. Some "features" are plain dangerous. 
Why does vector::operator[] not do a bounds check but happily overwrites 
non-owned memory? Why does map::operator[] with a new key silently insert a 
new element instead of complaining about a missing element? These are 
wonderful opportunities to shoot oneself into the foot.

Oryol has its own containers (after all, 3d engines and games need non-standard
containers anyway, like ring buffers or trees), but these are mostly compatible 
with <algorithm>.

I think that the behaviour of dynamic_cast is broken (returning a null-pointer
on a failed cast instead of terminating the app, there should be a '.isa' to
check for type compatibility), but it's the only sensible way to do downcasts
(from parent class down to derived class) without writing my own RTTI system 
(again). So for now standard RTTI is in, but I'm trying to avoid dynamic casts as much 
as possible. I would be happier if I would not need any RTTI at all (neither standard
or self-made).

The whole C++ streambuf stuff isn't used (this includes std::cout and std::cin).
The code bloat this introduces is incredible even if it isn't used. 

The rest of the std lib is mostly ok to use, but there's some stuff I'd like
to use, but compilers just aren't ready yet: constexpr isn't supported in 
Visual Studio, thread_local is a mess, chrono::high_resolution_clock isn't
high-resolution on Windows (unless millisecond accuracy is considered 'high-resolution').

Ok, enough C++ ranting, just keep in mind that not everything offered by
C++ is used in Oryol :)

