# _DESIGN MANIFESTO_ #

**this is work in progress**

Oryol aims to be a lean, experimental, multi-platform, C++11, 3D engine with focus on mobile- and web-platforms.

There is no roadmap, no timeline and no feature set.

## SCOPE ##

Oryol will at least provide the basic platform-wrapper services expected of a game engine:

* application model (entry point, game loop, lifetime events...)
* asset loading
* 3D rendering
* input
* audio

Oryol is extensible through statically linked code modules.

Oryol is a programmer's engine, not an artist's engine. But it can be the foundation of an artist's engine.


## LEAN ##

Oryol Apps:

- should be small and start fast
- should never require a big upfront download.
- should be self-contained executables with no external dependencies
- should not require installation (unless enforced by the host system)
- should not require administration rights (unless enforced by the host system)
- should be able to load required data from web servers (unless forbidden by the host system)


## EXPERIMENTAL ##

There will be a lot of change in Oryol, subsystems will be discarded and rewritten without warning, coding style will change, design guide lines will change. The use of C++ language features will change. Hopefully this will settle over time. 

One important reason why Oryol exists is that I can try out radical ideas not possible in a production engine like Nebula.


## MULTI-PLATFORM ##

Oryol should be able to run on anything with a CPU and GPU, but especially well on web- and mobile-platforms.

Running well on a huge number of feature- and performance-limited devices is better then looking superb on a very small number of high-end devices.

As such Oryol will always be limited by lower-end 3D APIs like OpenGL ES2. If a rendering technique is only possible with the latest desktop OpenGL or Direct3D version, it will very likely not be supported by Oryol.


## C++11 ##

Why C++:
* because Rust is not yet ready to replace C++
* because C/C++ is the only true multi-platform language
* because 'you don't pay for what you don't use'
* NOT because of its 'elegance' or the standard library

Oryol carefully chooses the C++ features it uses:
* if it requires exceptions enabled, it's out
* if it causes code bloat, it's out
* if it requires excessive dynamic memory allocation, it's out
