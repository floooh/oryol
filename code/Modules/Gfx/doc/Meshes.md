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
layout.Add(VertexAttr::Normal, VertexFormat::Float3);
layout.Add(VertexAttr::TexCoord0, VertexFormat::Float2);
```




### Mesh Creation

### Mesh Usage

### Immutable vs Dynamic Meshes

### Multiple Input Meshes

### Hardware Instancing

