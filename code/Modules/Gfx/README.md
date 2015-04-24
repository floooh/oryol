## Gfx Module

The Gfx module is the 3D rendering API of Oryol. 

### Getting Started

In general, the Oryol Gfx module tries to move all the complex resource and
render state setup out of the render loop and into the initialization phase.

In the initialization phase, the Gfx module itself is setup and graphics
resources likes textures, meshes and shaders are created. In the render
loop, resources and state required for the next draw call is applied, and
one of the draw methods is called to actually submit a draw call. Finally
the Gfx::CommitFrame() method is called which marks the end of the rendering
frame.

#### A minimal render loop

Here are code fragments for how to setup the Gfx system and
run a minimal render loop. For an actual working code example, see
the [Clear sample](http://floooh.github.com/oryol/Clear.html):


```cpp
#include "Gfx/Gfx.h"

// in App::OnInit(), initialize the Gfx module 
// (here a 400x300 window is created)
Gfx::Setup(GfxSetup::Window(400, 300, "Oryol Clear Sample"));
...

// in each invocation of App::OnRunning(), a single frame is rendered:
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
the App::OnInit() method. In each invocation of App::OnRunning(), one frame
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


#### Gfx Resources

Resources are opaque data objects required for rendering, like textures, meshes or shaders.

##### Resource Lifetime Management with Resource Labels

One important aspect of resource management is to tell the Gfx system when 
a resource is no longer needed to free up space for new resources. Instead of
complicated mechanisms like reference counting and dependency tracking, resources
in Oryol are explicitely bulk-destroyed by matching resource label.

Every resource object has a label assigned, with groups of related resources
having usually the same label. Resource labels are managed on a **label stack**,
new resources get the label at the top of the label stack assigned.

When a group of resources with the same label is no longer needed, a single
call to **Gfx::DestroyResources(label)** is used to bulk-destroy all
resources which have that label assigned.

The following Gfx method deal with resource labels:

```cpp
class Gfx {
    ...
    /// generate new resource label and push on label stack
    static ResourceLabel PushResourceLabel();
    /// push explicit resource label on label stack
    static void PushResourceLabel(ResourceLabel label);
    /// pop resource label from label stack
    static ResourceLabel PopResourceLabel();
    /// destroy one or several resources by matching label
    static void DestroyResources(ResourceLabel label);
...
};
```

##### Resource Creation

'Resource Creation' specifically means creating resource objects instantly from
data in memory. 

How a resource is created in detail is described by a small resource **Setup** 
object, optionally accompanied by a block of memory with actual data (for 
instance vertices, indices or texture image data).

The result of resource creation is an **Id**, which is an opaque handle
to the created resource. 

The following Gfx method are used for creating resources:

```cpp
class Gfx {
    ...
    /// create a resource object
    template<class SETUP> static Id CreateResource(const SETUP& setup);
    /// create a resource object with data
    template<class SETUP> static Id CreateResource(const SETUP& setup, const Ptr<Stream>& stream);
    /// create a resource object with data
    template<class SETUP> static Id CreateResource(const SetupAndStream<SETUP>& setupAndStream);
    ...    
};
```


##### Resource Loading

'Resource Loading' specifically means asynchronous creation of a resource
through a resource loader object, usually from a slow data source like
the hard disc or a web server.

There's only a single method for loading a resource which takes a 
resource loader and returns a resource Id:

```cpp
class Gfx {
    /// asynchronously load resource object
    static Id LoadResource(const Ptr<ResourceLoader>& loader);
};
```

The Id returned by Gfx::LoadResources() can be used immediately even though
the resource behind it isn't loaded yet. The Gfx module will either not
render the resource, or use a placeholder resource in case an 
non-existing resource is rendered.

##### Resource Sharing

The Gfx system provides detailed control over whether resources should
be shared or not through resource **Locators**. A Locator is a small
object consisting of a string name and an integer 'signature'. Two
Locator objects are identical if both the string name and the signature
match. The string name of a locator is usually the URL of a data
file on disk or on a web server, but can be anything else if a resource
is not loaded from a file system. 

Locator objects are usually embedded in resource **Setup** or **Resource Loader**
objects. When a resource is created through the Gfx::CreateResource()
or Gfx::LoadResource() methods, a lookup in an internal resource dictionary
(the 'resource registry') will be performed, and if there's already an
entry with identical Locator, the same resource will be returned

> NOTE: it is possible that a shared resource with a differenr ResourceLabel
is returned, with the consequence that a resource isn't destroyed when
this isn't actually intended. Sounds like a design bug, I'll have to
ponder this :)

It is also possible to lookup an existing resource by Locator
via the Gfx::LookupResource() method:

```cpp
class Gfx {
    ...
    /// lookup a resource Id by Locator
    static Id LookupResource(const Locator& locator);
    ...
};
```

##### Using Resources

The only way to 'use' a resource is through their opaque Id
handles returned by the creation, loading, and lookup methods.

Resources are either used as dependency of other resources (for
instance the DrawState resource holds a reference to a mesh
and to a shader program resource), or they are used in one of the
'apply' methods of the Gfx interface:


```cpp
class Gfx {
    ...
    /// apply an offscreen render target
    static void ApplyOffscreenRenderTarget(const Id& id);
    /// apply draw state to use for rendering
    static void ApplyDrawState(const Id& id);
    /// apply a shader constant block
    static void ApplyConstantBlock(const Id& id);
    /// apply a shader variable
    template<class T> static void ApplyVariable(int32 index, const T& value);
    ...
};
```

> NOTE: the Gfx::ApplyConstantBlock() is not yet implemented

> NOTE: the ApplyVariable() method only uses a resource Id when applying 
a texture resource to a shader parameter slot

##### Getting Information about Resources

Sometimes it is necessary to get back information about existing
resource, for instance if they have finished loading, or if
resource creation has failed. For this, the following 'query methods'
exist:

```cpp
class Gfx {
    ...
    /// query number of free slots for resource type
    static int32 QueryFreeResourceSlots(GfxResourceType::Code resourceType);
    /// query resource info (fast)
    static ResourceInfo QueryResourceInfo(const Id& id);
    /// query resource pool info (slow)
    static ResourcePoolInfo QueryResourcePoolInfo(GfxResourceType::Code resType);
    ...
}
```

> PERFORMANCE WARNING: the Gfx::QueryResourcePoolInfo() method is slow
and should not be called frequently (best to call this only once per
frame and only in a special debug mode)

> NOTE: minor details in the Query methods, like what methods actually
exists and their return types will most likely change

To check whether a resource is still loading, has finished loading, or
has failed to load, call Gfx::QueryResourceInfo() with the Id returned
by the resource creation method. The returned struct contains a 
State member with the current resource state. 

The method Gfx::QueryFreeResourceSlots() can be used to check how many
free slots are still available for a specific resource type. If
no more free slots exists, trying to create a new resource of that
type will fail.


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


