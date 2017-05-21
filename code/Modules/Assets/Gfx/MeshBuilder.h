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
    the Begin() method, and start writing vertices and indices,
    when done call End() and get the resulting SetupAndData object
    object which can be handed to Gfx::CreateResource().
    
    Vertex format packing happens on the fly when writing vertex data 
    according to the vertex layout given.
    
    This is the format of the stream data that will be written:
    
    [1..numVertices]
        [1..N bytes per vertex]
    [4-byte aligned, 1..numIndices]
        [2 or 4 bytes per index]
 
    @see VertexWriter, ShapeBuilder
*/
#include "Core/Types.h"
#include "Gfx/GfxTypes.h"
#include "Assets/Gfx/VertexWriter.h"
#include "Resource/SetupAndData.h"

namespace Oryol {

class MeshBuilder {
public:
    /// number of vertices
    uint32_t NumVertices = 0;
    /// number of indices (default 0 for non-indexed meshes)
    uint32_t NumIndices = 0;
    /// index type (default is 16-bit indices)
    IndexType::Code IndicesType = IndexType::Index16;
    /// read/write access to vertex layout
    class VertexLayout Layout;
    /// primitive groups (at least one must be defined)
    Array<PrimitiveGroup> PrimitiveGroups;
    /// vertex data usage
    Usage::Code VertexUsage = Usage::Immutable;
    /// index data usage
    Usage::Code IndexUsage = Usage::Immutable;
    
    /// begin writing vertex and index data
    MeshBuilder& Begin();
    /// write 1D component vertex data
    MeshBuilder& Vertex(uint32_t vertexIndex, VertexAttr::Code attr, float x);
    /// write 2D vertex data
    MeshBuilder& Vertex(uint32_t vertexIndex, VertexAttr::Code attr, float x, float y);
    /// write 3D vertex data
    MeshBuilder& Vertex(uint32_t vertexIndex, VertexAttr::Code attr, float x, float y, float z);
    /// write 4D vertex data
    MeshBuilder& Vertex(uint32_t vertexIndex, VertexAttr::Code attr, float x, float y, float z, float w);
    /// write 16-bit vertex-index at index-buffer-index
    MeshBuilder& Index(uint32_t index, uint16_t vertexIndex);
    /// write 32-bit vertex-index at index-buffer-index
    MeshBuilder& Index32(uint32_t index, uint32_t vertexIndex);
    /// write 16-bit triangle indices
    MeshBuilder& Triangle(uint32_t triangleIndex, uint16_t vertexIndex0, uint16_t vertexIndex1, uint16_t vertexIndex2);
    /// write 32-bit triangle indices
    MeshBuilder& Triangle32(uint32_t triangleIndex, uint32_t vertexIndex0, uint32_t vertexIndex1, uint32_t vertexIndex2);
    /// end writing vertex and index data, return result, and reset MeshBuilfer
    SetupAndData<MeshSetup> Build();

private:
    /// clear/reset the object
    void clear();
    /// compute byte offset into vertex buffer given vertex and component index
    uint32_t vertexByteOffset(uint32_t vertexIndex, int compIndex) const;

    SetupAndData<MeshSetup> setupAndData;
    bool inBegin = false;
    
    uint8_t* vertexPointer = nullptr;
    uint8_t* indexPointer = nullptr;
    uint8_t* endPointer = nullptr;
};

//------------------------------------------------------------------------------
inline MeshBuilder&
MeshBuilder::Index(uint32_t index, uint16_t vertexIndex) {
    o_assert_dbg(this->inBegin && (index < this->NumIndices) && (this->IndicesType == IndexType::Index16));
    o_assert_dbg(vertexIndex < this->NumVertices);
    
    uint16_t* ptr = ((uint16_t*)this->indexPointer) + index;
    o_assert_dbg(ptr < (uint16_t*)this->endPointer);
    *ptr = vertexIndex;
    return *this;
}
    
//------------------------------------------------------------------------------
inline MeshBuilder&
MeshBuilder::Index32(uint32_t index, uint32_t vertexIndex) {
    o_assert_dbg(this->inBegin && (index < this->NumIndices) && (this->IndicesType == IndexType::Index32));
    o_assert_dbg(vertexIndex < this->NumVertices);
    
    uint32_t* ptr = ((uint32_t*)this->indexPointer) + index;
    o_assert_dbg(ptr < (uint32_t*)this->endPointer);
    *ptr = vertexIndex;
    return *this;
}

//------------------------------------------------------------------------------
inline MeshBuilder&
MeshBuilder::Triangle(uint32_t triIndex, uint16_t vi0, uint16_t vi1, uint16_t vi2) {
    o_assert_dbg(this->inBegin && (triIndex*3 < this->NumIndices));
    o_assert_dbg((vi0 < this->NumVertices) && (vi1 < this->NumVertices) && (vi2 < this->NumVertices));
    
    uint16_t* ptr = ((uint16_t*)this->indexPointer) + triIndex * 3;
    o_assert_dbg(ptr < (uint16_t*)this->endPointer);
    ptr[0] = vi0; ptr[1] = vi1; ptr[2] = vi2;
    return *this;
}

//------------------------------------------------------------------------------
inline uint32_t
MeshBuilder::vertexByteOffset(uint32_t vertexIndex, int compIndex) const {
    o_assert_dbg(vertexIndex < this->NumVertices);
    o_assert_dbg(InvalidIndex != compIndex);
    return vertexIndex * this->Layout.ByteSize() + this->Layout.ComponentByteOffset(compIndex);
}
    
//------------------------------------------------------------------------------
inline MeshBuilder&
MeshBuilder::Vertex(uint32_t vertexIndex, VertexAttr::Code attr, float x) {
    o_assert_dbg(this->inBegin);
    const int compIndex = this->Layout.ComponentIndexByVertexAttr(attr);
    uint8_t* ptr = this->vertexPointer + this->vertexByteOffset(vertexIndex, compIndex);
    VertexWriter::Write(ptr, this->Layout.ComponentAt(compIndex).Format, x);
    return *this;
}
    
//------------------------------------------------------------------------------
inline MeshBuilder&
MeshBuilder::Vertex(uint32_t vertexIndex, VertexAttr::Code attr, float x, float y) {
    o_assert_dbg(this->inBegin);
    const int compIndex = this->Layout.ComponentIndexByVertexAttr(attr);
    uint8_t* ptr = this->vertexPointer + this->vertexByteOffset(vertexIndex, compIndex);
    VertexWriter::Write(ptr, this->Layout.ComponentAt(compIndex).Format, x, y);
    return *this;
}
    
//------------------------------------------------------------------------------
inline MeshBuilder&
MeshBuilder::Vertex(uint32_t vertexIndex, VertexAttr::Code attr, float x, float y, float z) {
    o_assert_dbg(this->inBegin);
    const int compIndex = this->Layout.ComponentIndexByVertexAttr(attr);
    uint8_t* ptr = this->vertexPointer + this->vertexByteOffset(vertexIndex, compIndex);
    VertexWriter::Write(ptr, this->Layout.ComponentAt(compIndex).Format, x, y, z);
    return *this;
}

//------------------------------------------------------------------------------
inline MeshBuilder&
MeshBuilder::Vertex(uint32_t vertexIndex, VertexAttr::Code attr, float x, float y, float z, float w) {
    o_assert_dbg(this->inBegin);
    const int compIndex = this->Layout.ComponentIndexByVertexAttr(attr);
    uint8_t* ptr = this->vertexPointer + this->vertexByteOffset(vertexIndex, compIndex);
    VertexWriter::Write(ptr, this->Layout.ComponentAt(compIndex).Format, x, y, z, w);
    return *this;
}

} // namespace Oryol
