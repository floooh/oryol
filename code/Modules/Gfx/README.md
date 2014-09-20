## Gfx Module

The Gfx module is the 3D rendering API of Oryol. 

### Getting Started

In general, the Oryol Gfx module tries to move all the complex resource and
render state setup out of the render loop and into the initialization phase.

In the initialization phase, the Gfx module itself is setup and graphics
resources likes textures, meshes and shaders are created. In the render
loop resources and state required for the next draw call is applied, and
one of the draw methods is called to actually submit a draw call. Finally
the Gfx::CommitFrame() method is called which marks the end of the rendering
frame.

#### A minimal render loop

Here are code fragments for how to setup the Gfx system and
run a minimal render loop. For an actual working code example, see
the [Clear sample](http://floooh.github.com/oryol/Clear.html):


```cpp
#include "Gfx/Gfx.h"

using namespace Gfx

// in App::OnInit(), initialize the Gfx module 
// (here a 400x300 window is created)
Gfx::Setup(GfxSetup::Window(400, 300, "Oryol Clear Sample"));
...

// in each invokation of App::OnRunning(), a single frame is rendered:
// - need to apply a render target (default render target in this case)
// - clear the render target to black
// - commit the rendered frame
Gfx::ApplyDefaultRenderTarget();
Gfx::Clear(PixelChannel::All, glm::vec4(0.0f));
Gfx::CommitFrame();

// at the end of OnRunning(), ask the Gfx system whether the app
// should continue running, or quit
return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;

// finally, in App::OnCleanup() we need to discard the Gfx module
Gfx::Discard();
```

#### Gfx initialization and teardown

Before an Oryol app can render anything, the Gfx module must be initialized in
the App::OnInit() method. In each invokation of App::OnRunning(), one frame
is rendered, finished with a call to Gfx::CommitFrame(). In App::OnCleanup()
the Gfx module is destroyed with a call to Gfx::Discard(). The Gfx::IsValid()
method can be used to check whether the Gfx module has been initialized:


```cpp
class Gfx {
    ...
    /// setup Gfx module
    static void Setup(const GfxSetup& setup);
    /// discard Gfx module
    static void Discard();
    /// check if Gfx module is setup
    static bool IsValid();    
    ...
}
```

The *GfxSetup* object allows to further customize the Gfx module and rendering environment. For
the most common display configuration, static construction methods exist (like windowed
versus fullscreen mode, multisample antialiasing on/off). The color and depth buffer
pixel formats can also be tweaked through public members.

Here's some sample code which initializes the default render target with 4xMSAA, 
an RGBA8 color buffer format and no depth/stencil buffer:

```cpp
auto gfxSetup = GfxSetup::WindowMSAA4(640, 480, "Window Title");
gfxSetup.ColorFormat = PixelFormat::RGBA8;
gfxSetup.DepthFormat = PixelFormat::None;
Gfx::Setup(gfxSetup);
```

For more Gfx module setup options, see the [GfxSetup header](https://github.com/floooh/oryol/blob/master/code/Modules/Gfx/Setup/GfxSetup.h).


#### Gfx Resources and Resource Handles

Resources are opaque data objects required for rendering, like textures, meshes or shaders.

The Oryol resource system is very simple on the outside, but fairly advanced on the inside.

The entire resource management is wrapped in 3 Gfx methods:

```cpp
class Gfx {
    /// create a resource, or return existing shared resource
    template<class SETUP> static GfxId CreateResource(const SETUP& setup);
    /// lookup a resource by resource locator
    static GfxId LookupResource(const Locator& locator);
    /// get the loading state of a resource
    static ResourceState::Code QueryResourceState(const GfxId& gfxId);
}
```

The most important method is *Gfx::CreateResource()*, this takes a *Setup Object*,
and returns a *GfxId* resource handle which represents the created resource object.

The Setup Object describes the type of the resource to be created, how it should be
created (for instance from a file, or a chunk of memory), and (usually) additional 
initialization attributes.

There is no explicite Gfx method to discard a resource. This is handled under the
hood by the *GfxId* object, which manages a resource use count (and thus works like a 
smart pointer to the resource). When the last *GfxId* of a resource is destroyed,
the resource itself is also destroyed.

Here are a few code samples demonstrating different ways to setup resources. However,
resource initialization is the most complex part of Oryol, and the resource setup 
objects have the biggest API surface. It is best to learn about all the different
ways to create resources by looking at the [Samples](https://github.com/floooh/oryol/tree/master/code/Samples/Gfx).

```cpp

// asynchronously load a texture from a DDS file 'tex:walls/brickwall.dds'
// the texture is automatically shared, if it is already loaded a handle 
// to the existing texture is returned
GfxId texId = Gfx::CreateResource(TextureSetup::FromFile("tex:walls/brickwall.dds"));

// load a texture, override the filter modes 
auto texSetup = TextureSetup::FromFile("tex:walls/brickwall.dds");
texSetup.MinFilter = TextureFilterMode::LinearMipmapLinear;
texSetup.MagFilter = TextureFilterMode::Linear;
GfxId texId = Gfx::CreateResource(texSetup);

// create a 128x128 render target texture, with 16-bit depth buffer,
// uv wrap-around and linear texture filtering
auto rtSetup = TextureSetup::RenderTarget(128, 128);
rtSetup.ColorFormat = PixelFormat::RGB8;
rtSetup.DepthFormat = PixelFormat::D16;
rtSetup.WrapU = TextureWrapMode::Repeat;
rtSetup.WrapV = TextureWrapMode::Repeat;
rtSetup.MagFilter = TextureFilterMode::Linear;
rtSetup.MinFilter = TextureFilterMode::Linear;
GfxId rtId = Gfx::CreateResource(rtSetup);
```

#### DrawState Objects

DrawState resource objects encapsulate all constant state required to submit 
a draw call:

- a mesh resource handle
- a shader program resource handle
- render state group objects (BlendState, DepthStencilState, RasterizerState)

FIXME: TO BE CONTINUED

#### Mesh Objects 
(TODO)

- vertex layout, packed vertex components
- indexed / non-indexed, 16 vs 32 bit indices
- primitive groups
- dynamic meshes
- instancing

#### Texture Objects
(TODO)

- pixel formats
- 2D, 3D, Cube Maps
- mipmapping, filtering, uv wrap mode
- compressed texture formats
- render targets, shared depth buffer, relative-sized
- setup from raw pixel data

#### ProgramBundle Objects
(TODO)

- variations
- uniform slots
- shader objects and reusing vertex/fragment shaders

### Shader Code Generator
(TODO)

### Advanced Topics

#### Resource Management

Some background information on resource management:

##### Resource States

A resource object goes through several states during its lifetime:

```cpp
class ResourceState {
    /// state codes
    enum Code {
        Initial,    ///< resource has just been created
        Setup,      ///< the resource has a setup object, but is not loaded
        Pending,    ///< resource is pending (asynchronous loading)
        Valid,      ///< resource has become valid
        Failed,     ///< resource creation has failed

        NumStates,
        InvalidState,
    };
};
```

The current state of a resource can be queried through the *Gfx::QueryResourceState()*
method using the resource handle returned at creation. Usually an application doesn't
need to care about resource states, if a resource isn't ready for rendering yet, nothing
will be rendered, or a placeholder resource.


##### Resource Pools

Resources are kept in fixed size resource pools, which means that only
a maximum number of resources of each type can exist at a time. The size of
these resource pools can be set in Gfx::Setup() through the GfxSetup object
with the 'GfxSetup::SetPoolSize() method.

##### Resource Locators and Sharing

(TODO)

##### Resource Throttling

Asynchronously resource creation can be 'throttled', which limits the maximum
number of resources of a specific type which will be created per frame. This helps
to take pressure from the resource system and reduces peak memory consumption
when many resource loading requests are issued in a very short time. The throttling
(max number of resources created per frame) is also defined in the GfxSetup object
via the 'GfxSetup::SetThrottling()' method.


