## Gfx Module

The Oryol Gfx module provides a thin portability wrapper around a 
platform's native 3D APIs, with WebGL/OpenGLES2 on the low-end, and
D3D12 and (hopefully soon) Vulkan on the 'high-end'.


### Platform/API matrix

What 3D APIs are supported on which platforms:

Platform   |GL3.3|GLES2|Metal|D3D11|D3D12|Vulkan
-----------|-----|-----|-----|-----|-----|------
OSX 10.11+ |YES  |---  |YES  |---  |---  |---
iOS 9.x+   |---  |YES  |YES  |---  |---  |---
Window7+   |YES  |---  |---  |YES  |---  |MAYBE
Window10+  |YES  |---  |---  |YES  |YES  |MAYBE
Linux      |YES  |---  |---  |---  |---  |MAYBE
Android    |---  |YES  |---  |---  |---  |MAYBE
HTML5      |---  |YES  |---  |---  |---  |---
PNaCl      |---  |YES  |---  |---  |---  |---
RaspberryPi|---  |YES  |---  |---  |---  |---

### Use the Source, Luke!

The most important and detailed documentation of the Gfx module
is the Samples source code. I'm trying very hard to make
the sample code easy to read and understand.

Looking at those four samples should give you most of the
information necessary to work with the Gfx module:

* [Clear Sample](https://github.com/floooh/oryol/blob/master/code/Samples/Clear/Clear.cc)
* [Triangle Sample](https://github.com/floooh/oryol/blob/master/code/Samples/Triangle/Triangle.cc)
* [Shapes Sample](https://github.com/floooh/oryol/blob/master/code/Samples/Shapes/Shapes.cc)
* [Texture Loading](https://github.com/floooh/oryol/blob/master/code/Samples/DDSCubeMap/DDSCubeMap.cc)

The 'manual' documentation in here will focus more on 'filling the gaps',
background information and explaining design choices.

### Selecting a Rendering Backend

Rendering backends are selected at compile time through cmake options and
cannot be switched at runtime. Each target platform has a default rendering
backend that will be selected automatically, usually this is OpenGL.

The easiest way to select a non-default rendering backend is to
select a different **fips build config**:

To see the list of supported build configs for D3D11, D3D12 
and Metal:

```
> ./fips list configs | grep d3d11
...
> ./fips list configs | grep d3d12
...
> ./fips list configs | grep metal
...
```

For instance, to select an iOS build config using the Metal rendering backend:

```
> ./fips set config metal-ios-xcode-debug
'config' set to 'metal-ios-xcode-debug' in project 'oryol'
> ./fips gen
...
> ./fips open
...
```

Alternatively, the rendering backend can be selected by activating
cmake options directly in the cmake config tool:

```bash
> fips config
...
# the cmake options are:

ORYOL_USE_D3D11
ORYOL_USE_D3D12
ORYOL_USE_METAL
```

### Frame Rendering

Rendering through the Gfx module usually involves the following steps:

1. [Initialize the Gfx Module](#initialize-the-gfx-module)
2. [Create Resources](#resources)
3. [Apply Render Target and optionally Clear](#render-targets)
4. [Optional: Update Dynamic Resources](#dynamic-resources)
5. [Apply a DrawState and Textures](#drawstates)
6. [Apply Uniform Blocks](#uniformblocks)
7. [Draw](#draw-functions)
8. [Commit](#committing-the-frame)
9. [Shutdown the Gfx Module](#shutting-down)

### Initialize the Gfx Module

To initialize the Gfx module, include the header **"Gfx/Gfx.h"** and
call the **Gfx::Setup()** method. This will create the application window
and a 3D-API context:

```cpp
#include "Gfx/Gfx.h"
...
    Gfx::Setup(GfxSetup::Window(800, 600, "My Oryol App"));
```
This would create a 800x600 window titled "My Oryol App".

The GfxSetup object allows to configure and tweak the rendering system
for the specific needs of the Oryol application. Have a look at the
[Gfx/Gfx.h](https://github.com/floooh/oryol/blob/master/code/Modules/Gfx/Setup/GfxSetup.h)
header to see what's possible.

>NOTE: On some platforms (mostly mobile), the actual rendering 
resolution can be different from what the application requested. To
get the actual rendering resolution, call the **Gfx::DisplayAttrs()** method after
the Gfx module has been initialized.

See also:
- [GfxSetup.h](https://github.com/floooh/oryol/blob/master/code/Modules/Gfx/Setup/GfxSetup.h)
- [Gfx.h](https://github.com/floooh/oryol/blob/master/code/Modules/Gfx/Gfx.h)

### Resources

Gfx resources are thin wrapper objects which manage the lifetime of an
underlying 3D-API specific rendering resource.

See also:
- [Resource Module](https://github.com/floooh/oryol/blob/master/code/Modules/Resource/README.md)

#### Resource Types

The Oryol Gfx module provides the following resource types:

1. [Meshes](#meshes): vertices, indices and primitive groups
2. [Textures](#textures): textures and offscreen-rendertargets
3. [Shaders](#shaders): vertex-shaders, fragment-shaders and shader parameters
4. [DrawStates](#drawstates): the complete state needed to issue draw-calls

The most important resource type is the DrawState, this bundles all static
state required for issuing draw calls. A DrawState takes one or several Meshes
and exactly one Shader as input, Textures are set together with a DrawState in
the Gfx::ApplyDrawState() call. Here's a diagram how the different resource
types are used together for rendering:

```
+--------------+
|              |
| 1..N Meshes  +-----+
|              |     |    +--------------+
+--------------+     |    |              |
                     +---->  DrawState   +----+
+--------------+     |    |              |    |
|              |     |    +--------------+    |
|    Shader    +-----+                        |
|              |                              |    +-------------------------+
+--------------+                              +---->  Gfx::ApplyDrawState()  |
                                              |    +------------+------------+
+---------------+                             |                 |
|               |                             |                 |
| 1..N Textures +-----------------------------+                 |
|               |                                  +------------v------------+
+---------------+                Shader Params+----> Gfx::ApplyUniformBlock()|
                                                   +------------+------------+
                                                                |
                                                                |
                                                   +------------v------------+
                                                   |       Gfx::Draw()       |
                                                   +-------------------------+
```

Most simple rendering scenarios will only use a single input Mesh. Using
multiple meshes is required for hardware-instanced rendering, and may 
be useful in other advanced scenarios, for instance if one part of the
vertex data is dynamic and another part is static.

The main reason why Meshes and Shaders are separate resource
objects, and not directly defined as part of a DrawState is reusability.
A single Mesh or Shader can be used by many different DrawStates.

The reason why Textures are not baked into DrawStates at all is to prevent
combinatorial explosions for the number of required DrawState object in some 
usage scenarios like using a cascade of offscreen render targets which are
then used as textures later in the rendering cascade.

Finally, modern rendering APIs have a lot of restrictions for dynamic state
that could change unpredictably during rendering. The relationship
between different resources and their memory layout is very
rigid and must be defined upfront. 

So basically, the way resources and their relationships have been designed in the
Oryol Gfx module is a compromise between keeping the whole system simple, the
requirements of modern 3D APIs, and the restrictions of the low-end APIs (WebGL
and OpenGLES2).

See also:
- [Gfx/Gfx.h](https://github.com/floooh/oryol/blob/master/code/Modules/Gfx/Gfx.h)

#### Resource Pools

All Gfx resources live in fixed-size pools, with each resource type having its
own pool. If a resource pool is full, creating additional resources of that
type will fail until some existing resources are destroyed. The resource pool
size for each resource type can be configured in the **GfxSetup** object at
startup time.

See also:
- [Gfx/Setup/GfxSetup.h](https://github.com/floooh/oryol/blob/master/code/Modules/Gfx/Setup/GfxSetup.h)
- [Resource/Core/ResourcePool.h](https://github.com/floooh/oryol/blob/master/code/Modules/Resource/Core/ResourcePool.h)

#### Resource Creation and Usage

Resource creation and usage always follows the same pattern:

1. fill-out a new **Setup** object which describes in detail how the resource
should be created
2. call one of the resource creation methods of the Gfx facade and get
a **Resource Id** back
3. use the returned Resource Id for rendering

Simple applications usually don't need to care much about resource destruction, all
resources will be properly destroyed at application shutdown. Apart from
shutdown, Oryol will never automatically destroy resources. See the **Resource
Lifetime Management** section below if more control over the lifetime
of resources is needed.

See also:

- [Resource/Id.h](https://github.com/floooh/oryol/blob/master/code/Modules/Resource/Id.h)

#### Resource Setup Objects

**Setup objects** describe in detail how a resource should be created, for instance
the size and pixel format of a texture, or how many vertices and indices are in
a mesh. They are the same concept as the DESC structures in D3D, or the
Descriptor objects in Metal.

Each resource type has it's own Setup class:

* [DrawStateSetup](https://github.com/floooh/oryol/blob/master/code/Modules/Gfx/Setup/DrawStateSetup.h)
* [MeshSetup](https://github.com/floooh/oryol/blob/master/code/Modules/Gfx/Setup/MeshSetup.h)
* [TextureSetup](https://github.com/floooh/oryol/blob/master/code/Modules/Gfx/Setup/TextureSetup.h)
* [ShaderSetup](https://github.com/floooh/oryol/blob/master/code/Modules/Gfx/Setup/ShaderSetup.h)

#### Resource Sharing

Resource sharing means that attempting to create an identical resource multiple times 
will return the original resource instead of creating a second copy of the
resource in memory.

Sharing is controlled through **Resource Locator** object. A resource locator
is a human-readable string which is both used as a sharing id, and as URL
for loadable resources. If a new resource is requested and another resource
with a matching resource locator already exists, the existing resource will be
returned instead of creating a new one.

Here is a very simple example of resource sharing:

```cpp
MeshSetup mshSetup = MeshSetup::CreateFromFile("msh:testmesh.omsh");
Id mesh0 = Gfx::LoadResource(MeshLoader::Create(mshSetup));
Id mesh1 = Gfx::LoadResource(MeshLoader::Create(mshSetup));
```

_mesh0_ and _mesh1_ will be identical, because they have both been
created with the same resource locator "msh:testmesh.omsh".

It is possible (although rarely needed) to explicitely suppress sharing,
even if the human-readable locator string is identical. For this reason,
resource locators have an additional _signature_ member. Two locators
are only equal, if both the locator string and signature match.

Let's modify the above example to suppress sharing with 2 different
signatures (1 and 2):

```
auto mshSetup0 = MeshSetup::CreateFromFile(Locator("msh:testmesh.omsh", 1));
auto mshSetup1 = MeshSetup::CreateFromFile(Locator("msh:testmesh.omsh", 2));
Id mesh0 = Gfx::LoadResource(MeshLoader::Create(mshSetup0));
Id mesh1 = Gfx::LoadResource(MeshLoader::Create(mshSetup1));
```

This will create 2 separate Mesh objects from the same file because the locator 
signatures _1_ and _2_ are different.

There is also a special signature constant **Locator::NonSharedSignature**
and a shortcut construction method **Locator::NonShared()** 
that causes locators to never match:

```
auto mshSetup = MeshSetup::CreateFromFile(Locator::NonShared("msh:testmesh.omsh"));
Id mesh0 = Gfx::LoadResource(MeshLoader::Create(mshSetup));
Id mesh1 = Gfx::LoadResource(MeshLoader::Create(mshSetup));
```

This guarantees that all Mesh objects created from the MeshSetup object _mshSetup_
to never be shared.

See also:
- [Resource/Locator.h](https://github.com/floooh/oryol/blob/master/code/Modules/Resource/Locator.h)

#### Resource Lifetime Management

Resource lifetime is managed explicitely by the application code with
**Resource Labels**. When a resource is created, it is automatically assigned
the top-most resource label from an internal resource label stack. Later,
when the resource is no longer needed, it is destroyed by calling
the method **Gfx::DestroyResources()** which takes a resource label as
argument.

One resource label is usually assigned to a whole group of related resource
objects. By calling the DestroyResources() method with this label, the
whole group of resources is destroyed with a single call.

Here's how it looks in (pseudo-)code, using an automatically generated resource
label:

```cpp
// push a new, automatically generated label on the label stack:
Gfx::PushResourceLabel();

// create one or more resources...
Id res0 = Gfx::CreateResource(...);
Id res1 = Gfx::CreateResource(...);
Id res2 = Gfx::CreateResource(...);

// pop the top-most label from the stack and store it somewhere
this->label = Gfx::PopResourceLabel();

// ... at some later point, destroy res0..res2 in a single call:
Gfx::DestroyResources(this->label);
```

It is also possible to push an explicitely defined resource label on 
the label stack, but you'll have to make sure that this doesn't collide
with other resource labels:

```cpp
// create an explicit resource label and push it on the label stack
const ResourceLabel myLabel(123456);
Gfx::PushResourceLabel(myLabel);
...
Gfx::PopResourceLabel();
...
Gfx::DestroyResources(myLabel);
```

See also:
- [Gfx/Gfx.h](https://github.com/floooh/oryol/blob/master/code/Modules/Gfx/Gfx.h)
- [Resource/ResourceLabel.h](https://github.com/floooh/oryol/blob/master/code/Modules/Resource/ResourceLabel.h)

#### Resource Creation vs Resource Loading

There are two ways to setup a new resource object in the Gfx module:

* **Gfx::CreateResource(...)**: create immediately from data in memory
* **Gfx::LoadResource(...)**: load asynchronously from disk/web

In the context of the Gfx module, **resource creation** always means that
a resource object is created immediately from existing data in memory, while
**resource loading** means that a resource is 'loaded' asynchronously from
a slow data source (for instance the hard disk or a web server).

The **Gfx::CreateResource()** methods take a resource Setup object, and 
optionally a chunk of memory, immediately create a usable resource object
and return a resource Id.

In contrast, the **Gfx::LoadResource()** method takes a pointer to a
user-provided **Resource Loader** object which knows how to load and create a
resource asynchronously, but immediately returns a usable resource Id.

The resource Id of a loading resource can be used immediately for rendering
even though the actual resource behind it doesn't exist yet (since the resource
data is still loading from the slow data source). The Gfx module will silently
ignore any draw calls that depend on a resource that is not in a valid
state (this includes: the resource is still loading, has failed to load, or
has been destroyed).

The Gfx module does not provide any specific **Resource Loader**
implementations, these have been moved out into the Assets module. An
application can also provide its own Resource Loaders, for instance to
load from custom file formats.

See also:
- [Gfx/Gfx.h](https://github.com/floooh/oryol/blob/master/code/Modules/Gfx/Gfx.h)
- [Resource/Core/ResourceLoader.h](https://github.com/floooh/oryol/blob/master/code/Modules/Resource/Core/ResourceLoader.h)

#### Resource States

A Gfx resource goes through different states during its lifetime. The state is
inspected by the Gfx module to decide whether all required resources for
a drawcall are valid, and it can be inspected by the application code with the
**Gfx::QueryResourceInfo()** call, for instance to check whether a resource is
still loading.

The resource states are:

* **ResourceState::Initial**: The resource object has just been created. This
  is a private state, user-code will never see a resource object in initial
  state.The only valid followup-state is the Setup state.
* **ResourceState::Setup**: The resource object has a valid Setup object
  assigned and is ready to be created. This is also an internal state,
  user-code will never encounter a resource object in the Setup-state. Valid
  followup states are Pending, Valid or Failed.
* **ResourceState::Pending**: The resource object is waiting for an asynchronous
IO operation to finish, this state is only set for resources created via
Gfx::LoadResource(). Possible followup states are Valid and Failed
* **ResourceState::Valid**: The resource has been created and is ready to be
used. 
* **ResourceState::Failed**: Creating the resource has failed (reasons could
be that a required file doesn't exist, or an invalid combination of 
Setup parameters was provided).

After a resource is destroyed, the associated resource object will not
be deleted (since it lives in a fixed-size resource pool), but will
simply go back into the Initial state.

See also:
- [Resource/ResourceState.h](https://github.com/floooh/oryol/blob/master/code/Modules/Resource/ResourceState.h)

#### Meshes

A Mesh resource object describes a piece of geometry required for rendering
and contains:

* a **vertex buffer** containing vertices
* a **vertex layout** describing the components of a vertex
* an optional **index buffer** containing 16- or 32-bit indices
* a **PrimitiveType** (whether the geometry is made of triangles, lines or
  points)
* an array of **PrimitiveGroups**

Meshes are not directly used for rendering, instead one or more Meshes
are used as input when creating a DrawState. More on that in the
DrawState section.

##### VertexLayout

A VertexLayout object describes how a vertex in the Mesh's vertex buffer is 
layed out in memory:

- the number and order of vertex components
- the meaning or 'semantic' of a each vertex component
- the data type of each vertex component

Let's say a vertex looks like this in memory:

- 3 floats for position
- 3 floats for the normal
- 2 floats for the texture coordinate

Setting up a matching vertex layout looks like this (calls to the
Add() method can be chained):

```cpp
VertexLayout layout;
layout.Add(VertexAttr::Position, VertexFormat::Float3)
      .Add(VertexAttr::Normal, VertexFormat::Float3)
      .Add(VertexAttr::TexCoord0, VertexFormat::Float2);
```

Oryol supports a number of packed vertex formats, for instance it is
quite usual to pack normal information into 4 bytes (1 byte required for
padding), and texture coordinates into 16-bit fixed-point values, 
for instance:

```cpp
VertexLayout layout;
layout.Add(VertexAttr::Position, VertexFormat::Float3)
      .Add(VertexAttr::Normal, VertexFormat::Byte4N)
      .Add(VertexAttr::TexCoord0, VertexFormat::Short2);
```

Vertex component packing uses less memory space and bandwidth when
pulling vertices into the vertex shader, so it should be always preferrable over
unpacked vertex data.

See also:
- [Gfx/Core/VertexLayout.h](https://github.com/floooh/oryol/blob/master/code/Modules/Gfx/Core/VertexLayout.h)
- [PackedNormals sample](https://github.com/floooh/oryol/blob/master/code/Samples/PackedNormals/PackedNormals.cc)


##### PrimitiveType and PrimitiveGroups

The Oryol Gfx module supports the following primitive types:

* triangle lists and strips
* line lists and strips
* points

This is the common subset of primitive types supported by all 3D-APIs.

A mesh can be split into several **PrimitiveGroups**, each group defines a
range of primitives in the mesh. A PrimitiveGroup is the smallest 'geometry 
unit' that can be rendered with one draw call. PrimitiveGroup are most commonly
used when a mesh is split up into several materials.

One mesh can only have a single primitive type, even though it may have
multiple primitive groups. This limitation was brought in by D3D12, where the
basic primitive type (triangles, lines or points) is part of the pipeline state
object.

See also:
- [Gfx/Core/PrimitiveGroup.h](https://github.com/floooh/oryol/blob/master/code/Modules/Gfx/Core/PrimitiveGroup.h)
- [Gfx/Core/Enums.h](https://github.com/floooh/oryol/blob/master/code/Modules/Gfx/Core/Enums.h)

##### Creating Meshes

Oryol provides 4 ways to create Mesh objects:

1. using the **MeshBuilder** class in the Assets module
2. using the **ShapeBuilder** class in the Assets module
3. loading from a .omsh file using the **MeshLoader**  class in the Assets
   module
4. creating from raw vertex- and index data in memory

The first 3 methods all build on the basic capability number 4 to create a 
Mesh object from raw data in memory

###### Creating a Mesh using the MeshBuilder class:

The **MeshBuilder** class (part of the Assets module) is useful for creating
meshes vertex-by-vertex with built-in support for vertex packing.

The following code creates a simple triangle mesh using the Asset module's
MeshBuilder class with per-vertex color
and no indices, taken from the [Triangle](https://github.com/floooh/oryol/blob/master/code/Samples/Triangle/Triangle.cc)
sample:

```cpp
#include "Assets/Gfx/MeshBuilder.h"
...

MeshBuilder meshBuilder;
meshBuilder.NumVertices = 3;
meshBuilder.IndicesType = IndexType::None;
meshBuilder.Layout
    .Add(VertexAttr::Position, VertexFormat::Float3)
    .Add(VertexAttr::Color0, VertexFormat::Float4);
meshBuilder.PrimitiveGroups.Add(0, 3);
meshBuilder.Begin()
    .Vertex(0, VertexAttr::Position, 0.0f, 0.5f, 0.5f)
    .Vertex(0, VertexAttr::Color0, 1.0f, 0.0f, 0.0f, 1.0f)
    .Vertex(1, VertexAttr::Position, 0.5f, -0.5f, 0.5f)
    .Vertex(1, VertexAttr::Color0, 0.0f, 1.0f, 0.0f, 1.0f)
    .Vertex(2, VertexAttr::Position, -0.5f, -0.5f, 0.5f)
    .Vertex(2, VertexAttr::Color0, 0.0f, 0.0f, 1.0f, 1.0f);
Id mesh = Gfx::CreateResource(meshBuilder.Build());
```
The MeshBuilder::Build() method returns a **SetupAndData<MeshSetup>** instance
which can be directly passed to the **Gfx::CreateResource()** method to
create the Mesh resource object.

See also:
- [Assets/Gfx/MeshBuilder.h](https://github.com/floooh/oryol/blob/master/code/Modules/Assets/Gfx/MeshBuilder.h)

###### Creating a Mesh using the ShapeBuilder class:

The **ShapeBuilder** class (part of the Assets module) builds on top of the
MeshBuilder class and can be used to build simple shapes (boxes, spheres,
cylinders, donuts and planes). This is mostly useful for debug-visualizations.

Here's a sample code snippet taken from the [PackedNormals](https://github.com/floooh/oryol/blob/master/code/Samples/PackedNormals/PackedNormals.cc)
sample building 5 different shapes as separate PrimitiveGroups into a single
Mesh object:

```cpp
#include "Assets/Gfx/ShapeBuilder.h"
...

ShapeBuilder shapeBuilder;
shapeBuilder.Layout
    .Add(VertexAttr::Position, VertexFormat::Float3)
    .Add(VertexAttr::Normal, VertexFormat::Byte4N);
shapeBuilder.Box(1.0f, 1.0f, 1.0f, 4)
    .Sphere(0.75f, 36, 20)
    .Cylinder(0.5f, 1.5f, 36, 10)
    .Torus(0.3f, 0.5f, 20, 36)
    .Plane(1.5f, 1.5f, 10);
Id mesh = Gfx::CreateResource(shapeBuilder.Build());
```

See also:
- [Assets/Gfx/ShapeBuilder.h](https://github.com/floooh/oryol/blob/master/code/Modules/Assets/Gfx/ShapeBuilder.h)

###### Loading mesh data from disk:

Finally, the Assets module also comes with an example mesh resource loader class
**MeshLoader** which can load .omsh files which can be created with the
oryol-export tool in the github project https://github.com/floooh/oryol-tools.

To load a mesh file from disk, the IO module must be initialized with 
at least one filesystem, for instance:

```cpp
IOSetup ioSetup;
ioSetup.FileSystems.Add("http", HTTPFileSystem::Creator());
ioSetup.Assigns.Add("msh:", "http://floooh.github.com/oryol/");
IO::Setup(ioSetup);
```

Loading a Mesh from disk is a one-liner:

```cpp
Id msh = Gfx::LoadResource(MeshLoader::Create(MeshSetup::CreateFromFile("msh:test.omsh")));
```

The returned Id can be used immediately, but rendering will be supressed
until the mesh data has been loaded and the mesh object behind the Id
has been created.

The MeshLoader class also allows to define a callback function which
is called after the mesh data has been loaded, but before the Mesh object
is created. This allows to inspect, and possibly change the MeshSetup
object that is going to be used to create the Mesh object.

Example code using a C++11 lambda function:

```cpp
Id msg = Gfx::LoadResource(MeshLoader::Create(MeshSetup::CreateFromFile("msg:test.omsh"), [](MeshSetup& setup) {
    // mesh data has been loaded, can now inspect and change
    // the provided MeshSetup object
    ...
    });
```

See also:
- [Assets/Gfx/MeshLoader.h](https://github.com/floooh/oryol/blob/master/code/Modules/Assets/Gfx/MeshLoader.h)
- [Assets/Gfx/OmshParser.h](https://github.com/floooh/oryol/blob/master/code/Modules/Assets/Gfx/OmshParser.h)
- [oryol-tools](https://github.com/floooh/oryol-tools)

#### Textures
TODO

#### Render Targets
TODO

#### Dynamic Resources
TODO

#### Shaders
TODO

##### UniformBlocks
TODO

#### DrawStates
TODO

### Draw Functions
TODO

### Committing a Frame
TODO

### Shutting Down
TODO

