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
is actually the sample source code. I'm trying very hard to make
the sample code easy to read and understand.

Looking at those four samples should give you most of the
information necessary to work with the Gfx module:

* [Clear Sample](https://github.com/floooh/oryol/blob/master/code/Samples/Clear/Clear.cc)
* [Triangle Sample](https://github.com/floooh/oryol/blob/master/code/Samples/Triangle/Triangle.cc)
* [Shapes Sample](https://github.com/floooh/oryol/blob/master/code/Samples/Shapes/Shapes.cc)
* [Texture Loading](https://github.com/floooh/oryol/blob/master/code/Samples/DDSCubeMap/DDSCubeMap.cc)

The 'manual' documentation in here will focus more on 'filling the gaps',
background information and design choices.

### Selecting a Rendering Backend

Rendering backends are selected at compile time through cmake
options. Each target platform has a default rendering backend
that will be selected automatically, usually this is OpenGL.

The easiest way to select a non-default rendering backend is to
select a different **fips build config**:

To see the list of supported build configs for D3D11, D3D12 
and Metal:

```
> fips list configs | grep d3d11
...
> fips list configs | grep d3d12
...
> fips list configs | grep metal
...
```

For instance, to select the Metal rendering backend for iOS:

```
> ./fips set config metal-ios-xcode-debug
'config' set to 'ios-xcode-debug' in project 'oryol'
> ./fips gen
...
> ./fips open
...
```

Alternatively, the rendering backend can be selected by setting
the cmake options directly in the cmake config tool:

```bash
> fips config
...
# the cmake options are:

ORYOL_USE_D3D11
ORYOL_USE_D3D12
ORYOL_USE_METAL
```

### Frame Rendering

The following steps are necessary to render something with the Gfx module:

1. [Initialize the Gfx Module](#initializing-the-gfx-module)
2. [Create Resources](#resources)
3. [Apply Render Target and optionally Clear](#render-targets)
4. [Optional: Update Dynamic Resources](#dynamic-resources)
5. [Apply a DrawState](#drawstates)
6. [Draw](#draw-functions)
7. [Commit](#committing-the-frame)
8. [Shutdown the Gfx Module](#shutting-down)

### Initializing the Gfx Module

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

>NOTE: On some platforms (especially mobile), the actual rendering 
resolution can be different from what the application requested. To
get the actual rendering resolution, call the **Gfx::DisplayAttrs()** after
the Gfx module has been initialized.

### Resources

Gfx resources are thin wrapper objects which manage the lifetime of an
underlying 3D-API specific rendering resource.

#### Resource Pools

All Gfx resources live in fixed-size pools, with each resource type having its
own pool. If a resource pool is full, creating additional resources will fail
until some existing resources are destroyed. The resource pool size for each
resource type can be configured in the **GfxSetup** object at startup time.

#### Resource Creation and Usage

Resource creation and usage always follows the same pattern:

1. fill-out a **Setup** object which describes in detail how the resource
should be created
2. call one of the resource creation methods of the Gfx facade and get
a **Resource Id** back
3. use the returned Resource Id for rendering

Simple application usually don't need to care about resource destruction, all
resources will be properly destroyed at application shutdown. Apart from
shutdown, Oryol will never automatically destroy resources. See the **Resource
Lifetime Management** section below if more control over the lifetime
of resources is needed.

#### Resource Setup Objects

**Setup objects** describe in detail how a resource should be created, for instance
the size and pixel format of a texture, or how many vertices and indices are in
a mesh. They are the same concept as the DESC structures in D3D, or the
Descriptor objects in Metal.

#### Resource Lifetime Management

Resource lifetime is managed explicitely by the application code with
**Resource Labels**. When a resource is created, it is automatically assigned
the top-most resource label from an internal resource label stack. Later
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

// pop the from the stack and store it somewhere
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
ignore any draw calls that depends on a resource that is not in a valid
state (this includes: the resource is still loading, has failed to load, or
has been destroyed).

The Gfx module does not provide any specific **Resource Loader**
implementations, these have been moved out into the Assets module. An
application can also provide its own Resource Loaders, for instance to
load from custom file formats.

#### Resource States

A Gfx resource goes through different states during its lifetime. The state is
inspected by the Gfx module itself to decide whether all required resources for
a drawcall are valid, and it can be inspected by the application code with the
**Gfx::QueryResourceInfo()** call, for instance to check whether a resource is
still loading.

#### Resource Types

1. [Meshes](#meshes): vertices, indices and primitive groups
2. [Textures](#textures): textures and offscreen-rendertargets
3. [Shaders](#shaders): vertex-shaders, fragment-shaders and shader parameters
4. [DrawStates](#drawstates): the complete state needed to issue draw-calls

#### Meshes

A Mesh resource object describes a piece of geometry required for rendering:

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

A VertexLayout object describes how a mesh vertex is layed out in memory:

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
quite usual to pack normal information into a 4 bytes (1 byte required for
padding), and texture coordinates into 16-bit fixed-point point values, 
for instance:

```cpp
VertexLayout layout;
layout.Add(VertexAttr::Position, VertexFormat::Float3)
      .Add(VertexAttr::Normal, VertexFormat::Byte4N)
      .Add(VertexAttr::TexCoord0, VertexFormat::Short2);
```

Vertex component packing uses less memory and less memory bandwidth when
pulling vertices into the vertex shader, so it is almost preferrable over
unpacked vertex data.

Have a look at the [PackedNormals](https://github.com/floooh/oryol/blob/master/code/Samples/PackedNormals/PackedNormals.cc)
sample to see vertex component packing in action.

##### PrimitiveType and PrimitiveGroups

The Oryol Gfx module supports the following primitive types:

* triangle lists and strips
* line lists and strips
* points

This is the common subset that is supported by all 3D-APIs. One mesh 
can only have a single primitive type even though a mesh can have
several primitive groups (== sub-meshes, see below). This limitation
was brought in by D3D12, where the primitive topology (triangles, lines or
points) is part of the pipeline state object.

A mesh can be split into several **PrimitiveGroups**, each group defines a
range of primitives in the mesh. A PrimitiveGroup is the smallest 'geometry 
unit' that can be rendered with one draw call. PrimitiveGroup are most commonly
used when a mesh is split up into several materials.

##### Working with Meshes



#### Textures
TODO

#### Render Targets
TODO

#### Dynamic Resources
TODO

#### Shaders
TODO

#### DrawStates
TODO

### Draw Functions
TODO

### Committing a Frame
TODO

### Shutting Down
TODO

