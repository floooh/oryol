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

Some vertex structures, and how the corresponding vertex layout is created:

```cpp
// a vertex with a position, normal and a single 2D texcoords, all floats
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
very useful to save memory and vertex fetching bandwidth. For instance it
often makes sense to pack normal data into a single Byte4N component (4
bytes instead of 3 for alignment reasons):

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
the itended update strategy of the mesh data:

* **Usage::Immutable**: the resource is initialized with data and cannot be
changed later, this is the most common and most efficient usage
* **Usage::Stream**: the resource is initialized without data, and the data
will be be updated by the CPU in each frame
* **Usage::Dynamic**: the resource is initialized without data and must
be written be the CPU before use, but the data update will be infrequently
(not per frame like in _Usage::Stream_)

Usage hints are provided independently for vertex and index data, see
the _Mesh Creation_ section below for more details.

#### Index Types

Vertex indices in the Oryol Gfx module can be either 16- or 32-bit. You
should always use 16-bit indices though unless 32-bit indices are definitely
required. 32-bit indices may have a performance penalty on some platforms,
and they require more bandwidth.

* **IndexType::None**: the mesh has no index data
* **IndexType::Index16**: the index data type is uint16_t
* **IndexType::Index32**: the index data type is uint32_t

#### Primitive Groups

A single mesh often contains vertex and index data for several unrelated
drawing operations. For instance the data in the mesh may be split into
different material groups, where each group must be rendered with 
a separate draw call. A **PrimitiveGroup** is a simple value pair
made of a _Base Element Index_ and _Element Count_ which together
define a group of primitives in the mesh data.

For indexed rendering the value pair describes a range of indices,
and for non-indexed rendering a range of vertices.

Multiple primitive groups can be associated with a mesh at creation time,
and used as used as parameter to the **Gfx::Draw()** method. This way the
rendering code doesn't need to know too many details about the underlying
mesh data.

It is also possible to create meshes without primitive groups, in this
case the drawing code needs pass a PrimitiveGroup object to
an overloaded version of **Gfx::Draw()**.

### Mesh Creation

Mesh creation follows the usual resource creation pattern:

* fill a MeshSetup object with creation parameters
* optionally setup the initial vertex- and index data
* call Gfx::CreateResource() and get an opaque Id back

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

#### Create a mesh with index data only

### Mesh Data Creation Helpers

### Mesh Usage

### Immutable vs Dynamic Meshes

### Multiple Input Meshes

### Hardware Instancing



