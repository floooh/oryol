## A Simple Render Loop

This document is a complete code-walkthrough of the most simple Gfx module
render loop (setup the Gfx module, clear the framebuffer to black each
frame, and shutdown the Gfx module).

The complete git repository for this simple
application is here: [https://github.com/floooh/oryol-simple/](https://github.com/floooh/oryol-simple/)

### Add a Linker Dependency

First add the Gfx module as linker dependency to your app's CMakeLists.txt file:

```
fips_begin_app(Simple windowed)
    ...
    fips_deps(Gfx)
fips_end_app()
```

### Render Loop Source Code

This is the entire source code for the simple
renderloop application:

```cpp
#include "Core/Main.h"
#include "Gfx/Gfx.h"

using namespace Oryol;

class SimpleApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();
};
OryolMain(SimpleApp);

AppState::Code SimpleApp::OnInit() {
    Gfx::Setup(GfxDesc().Width(640).Height(480).Title("Window Title"));
    return App::OnInit();
}

AppState::Code SimpleApp::OnRunning() {
    Gfx::BeginPass();
    Gfx::EndPass();
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

AppState::Code SimpleApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}
```

### Code Walkthrough

The minimal required demo harness for an Oryol application consists
of an App subclass with 3 override methods:

```cpp
#include "Core/Main.h"
#include "Gfx/Gfx.h"

using namespace Oryol;

class SimpleApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();
};
OryolMain(SimpleApp);
```

- **OnInit()**: performs initialization steps
- **OnRunning()**: the per-frame method where the actual rendering happens
- **OnCleanup()**: performs shutdown steps

In OnInit(), the call to **Gfx::Setup()** initializes the entire Gfx module,
it opens a window, and initializes the underlying 3D-API. The method takes a
**GfxDesc** object with setup parameters (more on that later).

```cpp
AppState::Code SimpleApp::OnInit() {
    Gfx::Setup(GfxDesc().Width(640).Height(480).Title("Window Title"));
    return App::OnInit();
}
```

All the interesting per-frame stuff happens in OnRunning(), which
is called per-frame:
```cpp
AppState::Code SimpleApp::OnRunning() {
    Gfx::BeginPass();
    Gfx::EndPass();
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}
```

The **Gfx::BeginPass()** call starts rendering to the default
framebuffer. The function is called without any arguments, this 
means the default 'pass actions' are applied: clear the
color buffer to black, the depth buffer with 1.0, and the stencil buffer
with 0.

The **Gfx::EndPass()** call finishes the currently active rendering
pass. This is only interesting insofar that in a more interesting program,
all actual rendering operations will happen inside a BeginPass/EndPass pair.

The **Gfx::CommitFrame** call tells the Gfx module that the current
rendering frame is complete and should be displayed.

Finally we need to decide whether the application should quit. In a more
complex application this might look a bit more involved, but in this
simple demo we're only checking if the user quits the application
through the host platform's window system (for instance by clicking
on the 'close window' button, pressing Alt-F4, or selecting the
Quit menu item). If this happens, the method **Gfx::QuitRequested()**
will return true, and the per-frame-callback will return 
AppState::Cleanup instead of AppState::Running, which causes the 
parent application class to switch to the Cleanup state (and thus
call the SimpleApp::OnCleanup() method):

```cpp
AppState::Code SimpleApp::OnCleanup() {
    Gfx::Discard();
    return App::OnCleanup();
}
```
The call to Gfx::Discard() shuts down the Gfx module, in a more complex
application this would also clean up all the left-over rendering resources.

### The GfxDesc object

The GfxDesc object provides setup parameters to the Gfx module, most 
importantly the window size, color- and depth-buffer formats, and the
sample count for multisample anti-aliasing.

The GfxDesc class uses method chaining to override the default parameters:

```cpp
auto gfxDesc = GfxDesc()
    .Width(1024)
    .Height(768)
    .ColorFormat(PixelFormat::RGBA8)
    .DepthFormat(PixelFormat::None)
    .SampleCount(4)
    .Windowed(true)
    .Title("My Window Title);
Gfx::Setup(gfxDesc);
```

The method chaining approach also lets you move the entire GfxDesc object
initialization into the Gfx::Setup() method call:

```cpp
Gfx::Setup(GfxDesc()
    .Width(1024)
    .Height(768)
    .ColorFormat(PixelFormat::RGBA8)
    .DepthFormat(PixelFormat::None)
    .SampleCount(4)
    .Windowed(true)
    .Title("My Window Title));
```

On platforms which support HighDPI rendering (e.g. Windows, macOS, iOS),
rendering happens by default at half resolution if a HighDPI display is
detected. To render at full resolution set the GfxDesc::HighDPI attribute
to true:

```cpp
Gfx::Setup(GfxDesc()
    ...
    .HighDPI(true)
    ...);
```

There's a number of GfxDesc members which tweak various
Gfx resource-system upper bounds. When starting with Oryol you don't
need to care about those values yet, but tweaking those values will
be useful for very big apps (which use a lot of resources), or very small app
which only use a handful resources.

Just for completeness, here are the GfxDesc members for resource
system tweaking and their default values:

```cpp
/// resource pool size by resource type
ResourcePoolSize(GfxResourceType::Code type, int size): default=128
/// initial resource label stack capacity
ResourceLabelStackCapacity(int capacity): default=256
/// initial resource registry capacity
ResourceRegistryCapacity(int capacity): default=256
/// size of the global uniform buffer (only relevant on some platforms)
GfxDesc& GlobalUniformBufferSize(int sizeInBytes): default=4MB
```

### The special HTML5 'canvas tracking' mode

There are 2 special GfxDesc members useful for HTML5 apps:

```cpp
/// if true, ignore own size and instead track size of an HTML element (emscripten only)
HtmlTrackElementSize(bool track): default = false
/// name of the HTML element to track
HtmlElement(const StringAtom& domName): default = "#canvas"
```

If **GfxDesc::HtmlTrackElementSize** is set to true, the size members
GfxDesc::Width and GfxDesc::Height are ignored, and the Oryol Gfx module
will instead track the size of the HTML DOM element identified by
**GfxDesc::HtmlElement** (the default value '#canvas' automatically
resolves to the WebGL canvas managed by the emscripten GL wrapper) . This is
useful for HTML5 apps where the WebGL canvas size is fully controlled by
Javascript code. The Oryol Gfx module needs to know when such changes take
place and adjust the rendering resolution accordingly.

More information about this special behaviour can be found in 
[this blogpost](http://floooh.github.io/2017/02/22/emsc-html.html).
