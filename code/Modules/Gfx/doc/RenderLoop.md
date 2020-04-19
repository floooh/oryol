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
    Gfx::Setup(GfxSetup::Window(640, 480, "Window Title"));
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
**GfxSetup** object with setup parameters (more on that later).

```cpp
AppState::Code SimpleApp::OnInit() {
    Gfx::Setup(GfxSetup::Window(640, 480, "Window Title"));
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

### The GfxSetup object

The GfxSetup object provides setup parameters to the Gfx module, most 
importantly the window size, color- and depth-buffer formats, and the
sample count for multisample anti-aliasing.

For the most common setup methods, a small set of construction methods
exist:

```cpp
class GfxSetup {
public:
    /// shortcut for windowed mode (with RGBA8, 24+8 stencil/depth, no MSAA)
    static GfxSetup Window(int width, int height, String windowTitle);
    /// shortcut for fullscreen mode (with RGBA8, 24+8 stencil/depth, no MSAA)
    static GfxSetup Fullscreen(int width, int height, String windowTitle);
    /// shortcut for windowed mode with 4xMSAA (with RGBA8, 24+8 stencil/depth)
    static GfxSetup WindowMSAA4(int width, int height, String windowTitle);
    /// shortcut for fullscreen mode with 4xMSAA (with RGBA8, 24+8 stencil/depth)
    static GfxSetup FullscreenMSAA4(int width, int height, String windowTitle);
...
}
```

You can also set or override the most common setup parameters manually:

```cpp
GfxSetup setup;
setup.Width = 1024;
setup.Height = 768;
setup.ColorFormat = PixelFormat::RGBA8;
setup.DepthFormat = PixelFormat::None;
setup.SampleCount = 4;
setup.Windowed = true;
setup.Title = "My Window Title"
Gfx::Setup(setup);
```

On platforms which support HighDPI rendering (e.g. Windows, macOS, iOS),
rendering happens by default at half resolution if a HighDPI display is
detected. To render at full resolution set the GfxSetup::HighDPI member to
true:

```cpp
GfxSetup setup;
...
setup.HighDPI = true;
...
Gfx::Setup(setup);
```

You can override the default 'pass action' (what should happen in BeginPass)
via the GfxSetup::DefaultPassAction member. For instance if you want to
clear the default framebuffer with color red instead of black:

```cpp
GfxSetup setup;
setup.DefaultPassAction = PassAction::Clear(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
...
```
...you can also change the clear values for the depth/stencil buffer from
the default 1.0 and 0:

```cpp
GfxSetup setup;
setup.DefaultPassAction = PassAction::Clear(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 0.5f, 255);
...
```

More information on render pass actions [is provided here](RenderPasses.md).

Finally there's a number of GfxSetup members which tweak various
Gfx resource-system upper bounds. When starting with Oryol you don't
need to care about those values yet, but tweaking those values will
be useful for very big apps (which use a lot of resources), or very small app
which only use a handful resources.

Just for completeness, here are the GfxSetup members for resource
system tweaking:

```cpp
/// resource pool size by resource type
StaticArray<int,GfxResourceType::NumResourceTypes> ResourcePoolSize;
/// resource creation throttling (max resources created async per frame)
StaticArray<int,GfxResourceType::NumResourceTypes> ResourceThrottling;
/// initial resource label stack capacity
int ResourceLabelStackCapacity = 256;
/// initial resource registry capacity
int ResourceRegistryCapacity = 256;
/// size of the global uniform buffer (only relevant on some platforms)
int GlobalUniformBufferSize = GfxConfig::DefaultGlobalUniformBufferSize;
/// max number of drawcalls per frame (only relevant on some platforms)
int MaxDrawCallsPerFrame = GfxConfig::DefaultMaxDrawCallsPerFrame;
/// max number of ApplyDrawState per frame (only relevant on some platforms)
int MaxApplyDrawStatesPerFrame = GfxConfig::DefaultMaxApplyDrawStatesPerFrame;
```

### The special HTML5 'canvas tracking' mode

There are 2 special GfxSetup members useful for HTML5 apps:

```cpp
/// if true, ignore own size and instead track size of an HTML element (emscripten only)
bool HtmlTrackElementSize = false;
/// name of the HTML element to track (default: "#canvas")
String HtmlElement = "#canvas";
```

If **GfxSetup::HtmlTrackElementSize** is set to true, the size members
GfxSetup::Width and GfxSetup::Height are ignored, and the Oryol Gfx module
will instead track the size of the HTML DOM element identified by
**GfxSetup::HtmlElement** (the default value '#canvas' automatically
resolves to the WebGL canvas managed by the emscripten GL wrapper) . This is
useful for HTML5 apps where the WebGL canvas size is fully controlled by
Javascript code. The Oryol Gfx module needs to know when such changes take
place and adjust the rendering resolution accordingly.

More information about this special behaviour can be found in 
[this blogpost](http://floooh.github.io/2017/02/22/emsc-html.html).
