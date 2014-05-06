## Render Module

NOTE: I really need to clean this up :/ The resource description overlaps with the Resource subsystem readme, and a "HOWTO" approach with sample code would probably be better...

The Render module is Oryol's low-level rendering API wrapper. It encapsulates an OpenGL-style rendering API into a platform-agnostic and easier to use C++ API.

### Concepts

#### The RenderFacade

The RenderFacade class is the public interface to the Render module. Before 3D rendering can start, a RenderFacade singleton object must be created which sets up an application window, the GL context and the default frame buffer.

The RenderFacade class interface is roughly split into 3 method groups:

* **Resource Management:** methods for creating and destroying resource objects
* **Apply Methods:** these methods apply state for the following Draw method
* **Draw Methods:** several methods to submit a draw call

#### Resources

Render resources are data objects necessary for rendering, for instance textures, meshes or shader programs. Oryol's resource management system is slightly higher-level then OpenGL or Direct3D:

- **transparent asynchronous resource loading:** All data loading happens asynchronously and behind the scenes, the caller doesn't need to care about (but can query) the current loading state of a resource. A resource handle can be used immediately for rendering, even if the resource is still loading, in this case, either a placeholder or nothing is rendered until all required resources for a drawing operation have become valid.
- **resource sharing**: Resource data which is already loaded will only have its use-count bumped. Sharing can be disabled if a unique copy of the resource data is needed.
- **fixed-size resource pools**: all Render resource objects live in pre-allocated, fixed-size resource pools, this reduces overhead for dynamic memory management and provides better control against "resource-creep"

##### Resource Creation and Destruction

All resource types are created and destroyed the same way:

1. create and initialize a resource **Setup** object which contains all the parameters necessary to create the resource object
2. call the **RenderFacade::CreateResource()** method with the Setup object as argument
3. the CreateResource() method will create a small **Resource::Id** object which uniquely identifies the resource object and which can immediately be used (even if the resource is still loading)
4. the CreateResource() method might either return a new Resource::Id, or an existing Resource::Id in case of a shared resource
5. after the resource is no longer needed, call the **RenderFactory::DiscardResource()** method, if the caller was the last user of the resource, the internal resource data will be released

##### Resource Setup Objects

The main purpose of resource setup objects is that they store the required setup parameters for the whole lifetime of a resource object to enable
automatic unloading and reloading of resource data as needed (for instance
for a resource streaming system, or more mundanely, because of a lost GL context).

Other nice side-effects of Setup objects are:

1. they simplify and unify the resource creation process
2. they can be stored, copied and generally handed around

The "keep all data around for creating an object" is not fully defined yet, if an object needs to be created from big, non-persistent data (for instance algorithmically created data), it would be wasteful to keep the creation data around as a copy in memory, and it would be more efficient to create the data again when needed. This problem isn't solved yet in Oryol.

##### The Resource Lifecycle

Resource object go through a sequence of lifecycle states (defined in the **Resource::State** enum class):

1. **Resource::State::Initial**: an empty resource object has been created but nothing else, a user of the Render module should never see a resource object in this state, the only valid followup-state is the *Setup* state
2. **Resource::State::Setup**: a Setup object has been attached but the resource initialization hasn't started, valid followup-states are *Pending*, *Valid* and *Failed*
3. **Resource::State::Pending**: resource creation / loading is currently underway, this state is only entered if the resource is loaded asynchronously, valid followup-states are *Valid* or *Failed*
4. **Resource::State::Valid**: the resource has been created and is ready for use, the only valid followup-state is *Setup*
5. **Resource::State::Failed**: initialization of the resource has failed, the only valid followup state is *Setup*

##### Resource Loaders

The Render module resource creation process can be customized by attaching Resource Loader objects to the RenderFacade. The typical usage for this is to support new asset file formats, but it can also be used to generally hook into the resource creation process and setup resources in a non-standard way.

The only restriction is that resource Setup object cannot be extended with new parameters currently.


### Sample Code

#### Setting up and shutting down the Rendering System:

This sets up the Render module with a 640x480 window for rendering with a default window title, and a default color- and depth-buffer:

```cpp
#include "Render/RenderFacade.h"
using namespace Oryol::Render
...
auto renderFacade = RenderFacade::CreateSingle(RenderSetup::Windowed(640, 480));
```

RenderFacade is a Singleton, so the CreateSingle() creator method must be used, and since singletons don't have automatic lifetime management, they must be destroyed explicitely at application shutdown:

```cpp
RenderFacade::DestroySingle();
```

We can also explicitely set a window title, and the format of the color- and depth buffer, and we don't have to cramp everything into one line of course:

```cpp
auto renderSetup = RenderSetup::Windowed(640, 480, "Title", PixelFormat::R8G8B8, PixelFormat::D24S8);
auto renderFacade = RenderFacade::CreateSingle(renderSetup);
```

To use only a color buffer, but not a depth buffer, use PixelFormat::None for the depth buffer argument:

```cpp
auto renderSetup = RenderSetup::Windowed(640, 460, "Title", PixelFormat::R8G8B8, PixelFormat::None);
auto renderFacade = RenderFacade::CreateSingle(renderSetup);
```

#### Working with Resources

##### Textures and Offscreen Render Targets

To asynchronously create a texture from an asset file:

```cpp
using namespace Oryol::Render;
using namespace Oryol::Resource;

// create texture from file at path 'tex:file.dds', if the texture is already
// loaded, we get a shared resource id back
auto texId = renderFacade->CreateResource(TextureSetup::FromFile("tex:file.dds"))

// ...use the new texture represented by 'texId'
...

// if no longer needed, discard the texture id
renderFacade->DiscardResource(texId);
```

Textures can also be created as render targets:

```cpp
auto rtSetup = TextureSetup::AsRenderTarget("rt", 128, 128, PixelFormat::R8G8B8, PixelFormat::D16);
auto rtId = renderFacade->CreateResource(rtSetup)
```

(...to be continued...)
