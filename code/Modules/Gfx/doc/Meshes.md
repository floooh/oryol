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
layout.Add(VertexAttr::Position, VertexFormat::Float3);
    .Add(VertexAttr::Normal, VertexFormat::Float3);
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
very useful to reduce memory usage and bandwidth. For instance it
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
* **Usage::Stream**: the resource is initialized without data, and the data
will be be updated by the CPU in each frame
* **Usage::Dynamic**: the resource is initialized without data and must
be written by the CPU before use, but updates will be infrequent
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
This way the rendering code doesn't need to know too many details about the
underlying mesh structure.

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

#### Create a mesh with dynamic vertex- and static index-data

TODO

#### Create a mesh with index data only

TODO

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
use the VertexWriter class (the header is _Assets/VertexWriter.h_):

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

TODO

### Hardware Instancing

TODO


