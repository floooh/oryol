## Meshes

Mesh objects provide the vertex- and index-data for rendering.

### Related Data Types

Before diving into meshes let's have a look at a few data types
that are important for creating and using meshes:

#### Vertex Layouts

In order to feed vertex data into the vertex shader, the 3D API needs
to know how a single vertex structure looks like, for instance: does
the vertex have texture coordinates? If yes, how many? Are the texture
coordinates provided as floating point number or packed integers? Are
the texture coordinates 1D, 2D or 3D? And so on...

This layout information is described in the **VertexLayout** class. A
vertex layout is a collection of **vertex components**, and each vertex
component consists of a **vertex attribute** (describing what the
vertex component is used for), and the **vertex format** (the data
type and number of values in the vertex component).

Here are some vertex structs, and how the corresponding
vertex layout object is created:

```cpp
// a vertex with a position, normal and a single 2D uv set, all floats
struct Vertex {
    float position[3];  
    float normal[3];
    float texcoord[2];
}

// build a matching vertex layout
VertexLayout layout;
layout.Add(VertexAttr::Position, VertexFormat::Float3);
layout.Add(VertexAttr::Normal, VertexFormat::Float3);
layout.Add(VertexAttr::TexCoord0, VertexFormat::Float2);
```

You can chain those Add calls, sometimes this is more convenient:

```cpp
VertexLayout layout;
layout.Add(VertexAttr::Position, VertexFormat::Float3)
    .Add(VertexAttr::Normal, VertexFormat::Float3)
    .Add(VertexAttr::TexCoord0, VertexFormat::Float2);
```

The VertexLayout class also supports C++11 initializer lists:

```cpp
VertexLayout layout0({
    { VertexAttr::Position, VertexFormat::Float3 },
    { VertexAttr::Normal, VertexFormat::Float3 },
    { VertexAttr::TexCoord0, VertexFormat::Float2 }
});

VertexLayout layout1;
layout1.Add({
    { VertexAttr::Position, VertexFormat::Float3 },
    { VertexAttr::Normal, VertexFormat::Float3 },
    { VertexAttr::TexCoord0, VertexFormat::Float2 }
});
```

The Oryol Gfx module supports a number of packed vertex formats, these are
very useful to reduce memory usage and vertex-fetch bandwidth. For instance it
often makes sense to pack normal data into a single Byte4N component (4
bytes because all vertex data must be 4-byte-aligned):

```cpp
struct PackedVertex {
    float position[3];
    int8_t packedNormal[4];
};

VertexLayout packedLayout({
    { VertexAttr::Position, VertexFormat::Float3 },
    { VertexAttr::Normal, VertexFormat::Byte4N },
});
```

#### Usage Hints

When creating a new mesh resource the Gfx module needs to be hinted about
the intended update strategy of the mesh data:

* **Usage::Immutable**: the resource is initialized with data and cannot be
changed later, this is the most common and most efficient usage
* **Usage::Stream**: the resource is initialized without data, but will be be 
updated by the CPU in each frame
* **Usage::Dynamic**: the resource is initialized without data and will
be written by the CPU before use, updates will be infrequent
(not per frame like in _Usage::Stream_)

Usage hints are provided independently for vertex and index data, see
the _Mesh Creation_ section below for more details.

#### Index Types

Vertex indices in the Oryol Gfx module can be either 16- or 32-bit. You
should always prefer 16-bit indices over 32-bit indices, since the latter
may have performance penalties on some platforms, and they take
up twice as much memory.

* **IndexType::None**: the mesh has no index data
* **IndexType::Index16**: the index data type is uint16_t
* **IndexType::Index32**: the index data type is uint32_t

#### Primitive Groups

A single mesh often contains vertex and index data for several unrelated
drawing operations. For instance the data in a mesh may be split into
different material groups, where each group must be rendered with 
a separate draw call. A **PrimitiveGroup** is a simple value pair
made of a _Base Element Index_ and _Element Count_ which together
define a group of primitives in the mesh data.

For indexed rendering the value pair describes a range of indices,
and for non-indexed rendering a range of vertices.

Multiple primitive groups can be associated with a mesh at creation time,
and a primitive-group-index used as parameter to the **Gfx::Draw()** method.
This way the rendering code doesn't need to know how exactly the mesh
data is split into groups.

It is also possible to create meshes without primitive groups, in this
case the drawing code needs to pass a PrimitiveGroup object to
an overloaded version of **Gfx::Draw()**.

### Mesh Creation

Mesh creation follows the usual resource creation pattern:

* fill a **MeshSetup** object with creation parameters
* optionally setup the initial vertex- and index data in memory
* call **Gfx::CreateResource()** and get an opaque Id back

Following are a number of examples for the most common scenarios:

#### Create mesh with vertex data from memory

This creates a triangle with vertex colors:

```cpp
const float vertices[] = {
    // positions            // colors (RGBA)
     0.0f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, 1.0f,
     0.5f, -0.5f, 0.5f,     0.0f, 1.0f, 0.0f , 1.0f,
    -0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f,
};
auto meshSetup = MeshSetup::FromData();
meshSetup.NumVertices = 3;
meshSetup.Layout = {
    { VertexAttr::Position, VertexFormat::Float3 },
    { VertexAttr::Color0, VertexFormat::Float4 }
};
meshSetup.AddPrimitiveGroup({0, 3});
this->drawState.Mesh[0] = Gfx::CreateResource(meshSetup, vertices, sizeof(vertices));
```

#### Create mesh with vertex and index data from memory

This creates a quad with 4 vertices and 6 indices (2 triangles). Since
the Gfx::CreateResource() function can only take a single data pointer the mesh
and indices must be defined in a single structure or memory block:

```cpp
// quad mesh with vertices followed by index data
static struct data_t {
    const float vertices[4 * 7] = {
        // positions            colors
        -0.5f,  0.5f, 0.5f,     1.0f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, 0.5f,     0.0f, 1.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.5f,     0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 0.0f, 1.0f,
    };
    const uint16_t indices[2 * 3] = {
        0, 1, 2,    // first triangle
        0, 2, 3,    // second triangle
    };
} data;

auto meshSetup = MeshSetup::FromData();
meshSetup.NumVertices = 4;
meshSetup.NumIndices = 6;
meshSetup.IndicesType = IndexType::Index16;
meshSetup.Layout = {
    { VertexAttr::Position, VertexFormat::Float3 },
    { VertexAttr::Color0, VertexFormat::Float4 }
};
meshSetup.AddPrimitiveGroup({0, 6});
meshSetup.VertexDataOffset = 0;
meshSetup.IndexDataOffset = offsetof(data_t, indices);
this->drawState.Mesh[0] = Gfx::CreateResource(meshSetup, &data, sizeof(data));
}
```

#### Create a mesh with dynamically updated data

To create a mesh with a dynamically updated vertex buffer, and
without index buffer:

```cpp
const int numVertices = 1024;
MeshSetup setup = MeshSetup::Empty(numVertices, Usage::Stream);
setup.Layout = {
    { VertexAttr::Position, VertexFormat::Float4 },
    { VertexAttr::Color0, VertexFormat::UByte4N }
};
Id mesh = Gfx::CreateResource(setup);
```

To create a mesh where both the vertex- and index-buffer are dynamically
updates:

```cpp
const int numVertices = 1024;
const int numIndices = 3 * numVertices;
MeshSetup meshSetup = MeshSetup::Empty(numVertices, Usage::Stream, IndexType::Index16, numIndices, Usage::Stream);
Id mesh = Gfx::CreateResource(setup);
```

It is also possible to create all other combinations, for instance:

- dynamically updated index buffer, no vertex buffer
- static vertex buffer, dynamically updated index buffer
- dynamic vertex buffer, static index buffer

The usage hint **Usage::Stream** means that the vertex- or index-buffers
are overwritten with new data **each frame**. If the data is updated
much less frequently, use the **Usage::Dynamic** hint.

To update the vertex- or index-data, call the methods **Gfx::UpdateVertices()** and/or **Gfx::UpdateIndex()**. These methods can only be called 
once per buffer per frame, and they need will overwrite the previous
content of the buffer. The update methods must be called in the same
frame *before* a draw call which needs the updated data

```cpp
// update vertex data from 'raw data'
const void* vertexData = ...;
const int vertexDataSizeInBytes = ...;
Gfx::UpdateVertices(mesh, vertexData, vertexDataSizeInBytes);

// ...and the same for index data
const void* indexData = ...;
const int indexDataSizeInBytes = ...;
Gfx::UpdateIndices(mesh, indexData, indexDataSizeInBytes);
```

### Mesh Data Creation Helpers

The Oryol Assets module has a few useful helper classes to generate mesh 
data:

- **ShapeBuilder**: create box, sphere, cylinder, torus and plane primitive shapes
- **MeshBuilder**: create arbitrary vertex and index data
- **VertexWriter**: write vertex data with automatic vertex component packing

The following ShapeBuilder code sample creates a single mesh with all possible
shape primitives, each in a separate primitive group (so that they can be
rendered with individual transforms). The code sample is taken from the
[PackedNormals sample](http://floooh.github.io/oryol/asmjs/PackedNormals.html):

```cpp
#include "Assets/Gfx/ShapeBuilder.h"
...

ShapeBuilder shapeBuilder;
shapeBuilder.Layout = {
    { VertexAttr::Position, VertexFormat::Float3 },
    { VertexAttr::Normal, VertexFormat::Byte4N }
};
shapeBuilder.Box(1.0f, 1.0f, 1.0f, 4)
    .Sphere(0.75f, 36, 20)
    .Cylinder(0.5f, 1.5f, 36, 10)
    .Torus(0.3f, 0.5f, 20, 36)
    .Plane(1.5f, 1.5f, 10);
this->drawState.Mesh[0] = Gfx::CreateResource(shapeBuilder.Build());
```

If you need more flexible mesh data creation, the lower level MeshBuilder class is the
next best option. Here is an example which creates a grid mesh with alternating vertex
colors. This is taken from the [PrimitiveTypes sample](http://floooh.github.io/oryol/asmjs/PrimitiveTypes.html). Note how the
vertex colors are provided as 4 floats, and automatically packed into an UByte4N
vertex format:

```cpp
#include "Assets/Gfx/MeshBuilder.h"

MeshBuilder meshBuilder;
meshBuilder.NumVertices = NumVertices;
meshBuilder.IndicesType = IndexType::None;
meshBuilder.Layout = {
    { VertexAttr::Position, VertexFormat::Float3 },
    { VertexAttr::Color0, VertexFormat::UByte4N }
};
meshBuilder.Begin();
const float dx = 1.0f / NumX;
const float dy = 1.0f / NumY;
const float xOffset = -dx * (NumX/2);
const float yOffset = -dy * (NumY/2);
for (int y = 0, vi=0; y < NumY; y++) {
    for (int x = 0; x < NumX; x++, vi++) {
        meshBuilder.Vertex(vi, VertexAttr::Position, x*dx+xOffset, y*dy+yOffset, 0.0f);
        switch (vi % 3) {
            case 0: meshBuilder.Vertex(vi, VertexAttr::Color0, 1.0f, 0.0f, 0.0f, 1.0f); break;
            case 1: meshBuilder.Vertex(vi, VertexAttr::Color0, 0.0f, 1.0f, 0.0f, 1.0f); break;
            default: meshBuilder.Vertex(vi, VertexAttr::Color0, 1.0f, 1.0f, 0.0f, 1.0f); break;
        }
    }
}
Id vertexMesh = Gfx::CreateResource(meshBuilder.Build());
```

If you just need a very low level way to write packed vertices to memory you can
use the VertexWriter class (the header is _Assets/Gfx/VertexWriter.h_):

```cpp
class VertexWriter {
public:
    /// write 1D generic vertex component with run-time pack-format selection
    static uint8_t* Write(uint8_t* dst, VertexFormat::Code fmt, float x);
    /// write 2D generic vertex component with run-time pack-format selection
    static uint8_t* Write(uint8_t* dst, VertexFormat::Code fmt, float x, float y);
    /// write 3D generic vertex component with run-time pack-format selection
    static uint8_t* Write(uint8_t* dst, VertexFormat::Code fmt, float x, float y, float z);
    /// write 4D generic vertex component with run-time pack-format selection
    static uint8_t* Write(uint8_t* dst, VertexFormat::Code fmt, float x, float y, float z, float w);
};
```

### Multiple Input Meshes

A draw call can feed its vertex data from up to 4 different input
meshes, this may make sense to reduce the amount of data that
needs to be written by the CPU, or read by the GPU in some
situations:

- *mixed static and dynamic vertex data*: Let's say you want 3d geometry
where the position and surface normal vertex components are static,
but the texture coordinates should be generated each frame by the
CPU. In this case you should create 2 meshes: one with all static 
components (including the index buffer), and another mesh with the
dynamic vertex buffer for texture coordinates.
- *optional vertex components*: Not all vertex components of a mesh 
are needed in all render passes, for instance in a shadow pass, 
the surface normals are usually not needed, only the the vertex positions.
In this case it may make sense to group vertex components into different
vertex buffers by usage (for instance the positions would go into
one vertex buffer, and all other components into a second vertex buffer)
- *hardware instancing*: hardware instancing requires at least 2
meshes, the first mesh with the static 'model data' (the usual
vertex- and index-data), and the second mesh with an instance-data
vertex buffer, which has one vertex per instance to be rendered.

The following sample pseudo-code creates 2 meshes, one with
positions and another one with colors. Finally a pipeline object
is created which needs to know how a combined vertex looks like
by setting the original mesh vertex layouts into 'bind slots'
in the PipelineSetup object:

```cpp
DrawState drawState;

// first mesh with positions into the first mesh bind slot
float posData[] = { ... };
MeshSetup posMeshSetup = MeshSetup::FromData();
posMeshSetup.Layout = {
    { VertexAttr::Position, VertexFormat::Float3 }
};
drawState.Mesh[0] = Gfx::CreateResource(posSetup, posData, sizeof(posData));

// second mesh with color data into the second mesh bind slot
float clrData[] = { ... };
MeshSetup clrMeshSetup = MeshSetup::FromData();
clrMeshSetup = MeshSetup::FromData();
clrMeshSetup.Layout = { 
    { VertexAttr::Color0, VertexFormat::Float4 }
};
drawState.Mesh[1] = Gfx::CreateResource(clrSetup, clrData, sizeof(clrData));

// pipeline object needs to know combined layouts which form a vertex
PipelineSetup pipSetup = PipelineSetup::FromShader(shd);
pipSetup.Layouts[0] = posMeshSetup.Layout;
pipSetup.Layouts[1] = clrMeshSetup.Layout;
...
drawState.Pipeline = Gfx::CreateResource(pipSetup);
```

### Hardware Instancing

Hardware-instancing uses one mesh for the geometry to be instanced 
(vertex-buffer only, or vertex+index buffer), and a second mesh
with a per-instance-data vertex buffer. The instance data buffer
is usually created as dynamic buffer so that the CPU can update
it with new instance-data each frame.

The following code sample to create the 2 meshes is taken from the
[Instancing sample](https://github.com/floooh/oryol/blob/master/code/Samples/Instancing/Instancing.cc):

```cpp
// create static mesh at mesh slot 0
const glm::mat4 rot90 = glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
ShapeBuilder shapeBuilder;
shapeBuilder.RandomColors = true;
shapeBuilder.Layout = {
    { VertexAttr::Position, VertexFormat::Float3 },
    { VertexAttr::Color0, VertexFormat::Float4 }
};
shapeBuilder.Transform(rot90).Sphere(0.05f, 3, 2);
auto shapeBuilderResult = shapeBuilder.Build();
this->drawState.Mesh[0] = Gfx::CreateResource(shapeBuilderResult);

// create dynamic instance data mesh at mesh slot 1
auto instMeshSetup = MeshSetup::Empty(MaxNumParticles, Usage::Stream);
instMeshSetup.Layout
    .EnableInstancing()
    .Add(VertexAttr::Instance0, VertexFormat::Float4);
this->drawState.Mesh[1] = Gfx::CreateResource(instMeshSetup);
```

Note the **EnableInstancing()** call in the vertex layout definition
of the second mesh. This tells the layout that the instance data
has a different 'vertex step rate' than the geometry vertex data.

When creating the pipeline state object for instanced rendering,
the vertex layouts must be set into the PipelineSetup layout
slots:

```cpp
auto ps = PipelineSetup::FromShader(shd);
ps.Layouts[0] = shapeBuilder.Layout;
ps.Layouts[1] = instMeshSetup.Layout;
...
this->drawState.Pipeline = Gfx::CreateResource(ps);
```
