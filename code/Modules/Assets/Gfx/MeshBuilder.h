#pragma once
//------------------------------------------------------------------------------
/**
    @defgroup Assets Assets
    @brief asset creation and loading umbrella module

    @class Oryol::MeshBuilder
    @ingroup Assets
    @brief build mesh data programmatically
    
    The MeshBuilder class simplifies creating mesh data from scratch with
    the CPU. It is mainly designed for convenience, less for performance,
    so it should mainly be used to setup static, immutable geometry. For
    dynamic geometry which needs to change every frame lower level approaches
    should be used (such as VertexWriter). To create pre-defined shapes
    such as cube, sphere or donuts, consider using the higher level 
    ShapeBuilder class which is built on top of MeshBuilder.
    
    In order to start writing vertices and indices, first tell the
    MeshBuilder the number of vertices and their layout, the number of
    indices (in case of indexed geometry), the index type (16- or
    32-bits) and finally the primitive group definitions, then call
    the Build() method, and in the callback function write vertices and indices.
    The MeshBuilder::Result object returned by the Build() method contains
    all the required information to create buffers and pipeline
    objects needed to render the mesh data.

    Vertex format packing happens on the fly when writing vertex data 
    according to the vertex layout given.
    
    @see VertexWriter, ShapeBuilder
*/
#include "Core/Types.h"
#include "Gfx/GfxTypes.h"
#include "Assets/Gfx/VertexWriter.h"
#include <functional>

namespace Oryol {

class MeshBuilder {
public:
    MeshBuilder& NumVertices(int n)             { numVertices = n; return *this; }
    int NumVertices() const                     { return numVertices; }
    MeshBuilder& NumIndices(int n)              { numIndices = n; return *this; }
    int NumIndices() const                      { return numIndices; }
    MeshBuilder& IndexType(Oryol::IndexType::Code t) { indexType = t; return *this; }
    Oryol::IndexType::Code IndexType() const    { return indexType; }
    MeshBuilder& Layout(const VertexLayout& l)  { layout = l; return *this; }
    const VertexLayout& Layout() const          { return layout; }
    VertexLayout& Layout()                      { return layout; }
    MeshBuilder& VertexUsage(Usage::Code u)     { vertexUsage = u; return *this; }
    Usage::Code VertexUsage() const             { return vertexUsage; }
    MeshBuilder& IndexUsage(Usage::Code u)      { indexUsage = u; return *this; }
    Usage::Code IndexUsage() const              { return indexUsage; }
    struct Result {
        BufferDesc VertexBufferDesc;
        BufferDesc IndexBufferDesc;
        VertexLayout Layout;
        Oryol::IndexType::Code IndexType;
        MemoryBuffer Data;
    };
    Result Build(std::function<void(MeshBuilder& mb)> func);

    /// these functions must be called from within the Build() callback
    MeshBuilder& Vertex(int vertexIndex, int compIndex, float x);
    MeshBuilder& Vertex(int vertexIndex, int compIndex, float x, float y);
    MeshBuilder& Vertex(int vertexIndex, int compIndex, float x, float y, float z);
    MeshBuilder& Vertex(int vertexIndex, int compIndex, float x, float y, float z, float w);
    MeshBuilder& Index(int index, uint16_t vertexIndex);
    MeshBuilder& Index32(int index, uint32_t vertexIndex);
    MeshBuilder& Triangle(int triangleIndex, uint16_t vertexIndex0, uint16_t vertexIndex1, uint16_t vertexIndex2);
    MeshBuilder& Triangle32(int triangleIndex, uint32_t vertexIndex0, uint32_t vertexIndex1, uint32_t vertexIndex2);

private:
    void clear();
    int vertexByteOffset(int vertexIndex, int compIndex) const;

    int numVertices = 0;
    int numIndices = 0;
    IndexType::Code indexType = IndexType::UInt16;
    Usage::Code vertexUsage = Usage::Immutable;
    Usage::Code indexUsage = Usage::Immutable;
    VertexLayout layout;

    bool inBuild = false;
    uint8_t* vertexPointer = nullptr;
    uint8_t* indexPointer = nullptr;
    uint8_t* endPointer = nullptr;
};

//------------------------------------------------------------------------------
inline MeshBuilder&
MeshBuilder::Index(int index, uint16_t vertexIndex) {
    o_assert_dbg(this->inBuild && (index < this->numIndices) && (this->indexType == IndexType::UInt16));
    o_assert_dbg(vertexIndex < this->numVertices);
    
    uint16_t* ptr = ((uint16_t*)this->indexPointer) + index;
    o_assert_dbg(ptr < (uint16_t*)this->endPointer);
    *ptr = vertexIndex;
    return *this;
}
    
//------------------------------------------------------------------------------
inline MeshBuilder&
MeshBuilder::Index32(int index, uint32_t vertexIndex) {
    o_assert_dbg(this->inBuild && (index < this->numIndices) && (this->indexType == IndexType::UInt32));
    o_assert_dbg((int)vertexIndex < this->numVertices);
    
    uint32_t* ptr = ((uint32_t*)this->indexPointer) + index;
    o_assert_dbg(ptr < (uint32_t*)this->endPointer);
    *ptr = vertexIndex;
    return *this;
}

//------------------------------------------------------------------------------
inline MeshBuilder&
MeshBuilder::Triangle(int triIndex, uint16_t vi0, uint16_t vi1, uint16_t vi2) {
    o_assert_dbg(this->inBuild && (triIndex*3 < this->numIndices));
    o_assert_dbg((vi0 < this->numVertices) && (vi1 < this->numVertices) && (vi2 < this->numVertices));
    
    uint16_t* ptr = ((uint16_t*)this->indexPointer) + triIndex * 3;
    o_assert_dbg(ptr < (uint16_t*)this->endPointer);
    ptr[0] = vi0; ptr[1] = vi1; ptr[2] = vi2;
    return *this;
}

//------------------------------------------------------------------------------
inline int
MeshBuilder::vertexByteOffset(int vertexIndex, int compIndex) const {
    o_assert_dbg(vertexIndex < this->numVertices);
    o_assert_dbg(InvalidIndex != compIndex);
    return vertexIndex * this->layout.ByteSize() + this->layout.ComponentByteOffset(compIndex);
}
    
//------------------------------------------------------------------------------
inline MeshBuilder&
MeshBuilder::Vertex(int vertexIndex, int compIndex, float x) {
    o_assert_dbg(this->inBuild);
    uint8_t* ptr = this->vertexPointer + this->vertexByteOffset(vertexIndex, compIndex);
    VertexWriter::Write(ptr, this->layout.ComponentAt(compIndex).Format, x);
    return *this;
}
    
//------------------------------------------------------------------------------
inline MeshBuilder&
MeshBuilder::Vertex(int vertexIndex, int compIndex, float x, float y) {
    o_assert_dbg(this->inBuild);
    uint8_t* ptr = this->vertexPointer + this->vertexByteOffset(vertexIndex, compIndex);
    VertexWriter::Write(ptr, this->layout.ComponentAt(compIndex).Format, x, y);
    return *this;
}
    
//------------------------------------------------------------------------------
inline MeshBuilder&
MeshBuilder::Vertex(int vertexIndex, int compIndex, float x, float y, float z) {
    o_assert_dbg(this->inBuild);
    uint8_t* ptr = this->vertexPointer + this->vertexByteOffset(vertexIndex, compIndex);
    VertexWriter::Write(ptr, this->layout.ComponentAt(compIndex).Format, x, y, z);
    return *this;
}

//------------------------------------------------------------------------------
inline MeshBuilder&
MeshBuilder::Vertex(int vertexIndex, int compIndex, float x, float y, float z, float w) {
    o_assert_dbg(this->inBuild);
    uint8_t* ptr = this->vertexPointer + this->vertexByteOffset(vertexIndex, compIndex);
    VertexWriter::Write(ptr, this->layout.ComponentAt(compIndex).Format, x, y, z, w);
    return *this;
}

} // namespace Oryol
