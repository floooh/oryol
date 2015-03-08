#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::MeshBuilder
    @ingroup Asset
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
    when done call End() and get the resulting Stream object
    object which can be handed together with a MeshSetup object to 
    Gfx::CreateResource().
    
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
#include "Gfx/Core/Enums.h"
#include "Gfx/Core/VertexLayout.h"
#include "Gfx/Core/PrimitiveGroup.h"
#include "Gfx/Setup/MeshSetup.h"
#include "Assets/Gfx/VertexWriter.h"
#include "IO/Stream/MemoryStream.h"
#include "Resource/Core/SetupAndStream.h"

namespace Oryol {

class MeshBuilder {
public:
    /// constructor
    MeshBuilder();
    
    /// number of vertices
    uint32 NumVertices;
    /// number of indices (default 0 for non-indexed meshes)
    uint32 NumIndices;
    /// index type (default is 16-bit indices)
    IndexType::Code IndicesType;
    /// read/write access to vertex layout
    class VertexLayout Layout;
    /// primitive groups (at least one must be defined)
    Array<PrimitiveGroup> PrimitiveGroups;
    /// vertex data usage
    Usage::Code VertexUsage;
    /// index data usage
    Usage::Code IndexUsage;
    
    /// begin writing vertex and index data
    MeshBuilder& Begin();
    /// write D component vertex data
    MeshBuilder& Vertex(uint32 vertexIndex, VertexAttr::Code attr, float32 x);
    /// write 2D vertex data
    MeshBuilder& Vertex(uint32 vertexIndex, VertexAttr::Code attr, float32 x, float32 y);
    /// write 3D vertex data
    MeshBuilder& Vertex(uint32 vertexIndex, VertexAttr::Code attr, float32 x, float32 y, float32 z);
    /// write 4D vertex data
    MeshBuilder& Vertex(uint32 vertexIndex, VertexAttr::Code attr, float32 x, float32 y, float32 z, float32 w);
    /// write 16-bit vertex-index at index-buffer-index
    MeshBuilder& Index(uint32 index, uint16 vertexIndex);
    /// write 32-bit vertex-index at index-buffer-index
    MeshBuilder& Index32(uint32 index, uint32 vertexIndex);
    /// write 16-bit triangle indices
    MeshBuilder& Triangle(uint32 triangleIndex, uint16 vertexIndex0, uint16 vertexIndex1, uint16 vertexIndex2);
    /// write 32-bit triangle indices
    MeshBuilder& Triangle32(uint32 triangleIndex, uint32 vertexIndex0, uint32 vertexIndex1, uint32 vertexIndex2);
    /// end writing vertex and index data
    void End();
    /// get result
    const SetupAndStream<MeshSetup>& Result() const;
    /// clear the mesh builder
    void Clear();
    
private:
    /// compute byte offset into vertex buffer given vertex and component index
    uint32 vertexByteOffset(uint32 vertexIndex, int32 compIndex) const;
    
    SetupAndStream<MeshSetup> setupAndStream;
    bool inBegin;
    bool resultValid;
    
    uint8* vertexPointer;
    uint8* indexPointer;
    uint8* endPointer;
};

//------------------------------------------------------------------------------
inline MeshBuilder&
MeshBuilder::Index(uint32 index, uint16 vertexIndex) {
    o_assert_dbg(this->inBegin && (index < this->NumIndices) && (this->IndicesType == IndexType::Index16));
    o_assert_dbg(vertexIndex < this->NumVertices);
    
    uint16* ptr = ((uint16*)this->indexPointer) + index;
    o_assert_dbg(ptr < (uint16*)this->endPointer);
    *ptr = vertexIndex;
    return *this;
}
    
//------------------------------------------------------------------------------
inline MeshBuilder&
MeshBuilder::Index32(uint32 index, uint32 vertexIndex) {
    o_assert_dbg(this->inBegin && (index < this->NumIndices) && (this->IndicesType == IndexType::Index32));
    o_assert_dbg(vertexIndex < this->NumVertices);
    
    uint32* ptr = ((uint32*)this->indexPointer) + index;
    o_assert_dbg(ptr < (uint32*)this->endPointer);
    *ptr = vertexIndex;
    return *this;
}

//------------------------------------------------------------------------------
inline MeshBuilder&
MeshBuilder::Triangle(uint32 triIndex, uint16 vi0, uint16 vi1, uint16 vi2) {
    o_assert_dbg(this->inBegin && (triIndex*3 < this->NumIndices));
    o_assert_dbg((vi0 < this->NumVertices) && (vi1 < this->NumVertices) && (vi2 < this->NumVertices));
    
    uint16* ptr = ((uint16*)this->indexPointer) + triIndex * 3;
    o_assert_dbg(ptr < (uint16*)this->endPointer);
    ptr[0] = vi0; ptr[1] = vi1; ptr[2] = vi2;
    return *this;
}

//------------------------------------------------------------------------------
inline uint32
MeshBuilder::vertexByteOffset(uint32 vertexIndex, int32 compIndex) const {
    o_assert_dbg(vertexIndex < this->NumVertices);
    o_assert_dbg(InvalidIndex != compIndex);
    return vertexIndex * this->Layout.ByteSize() + this->Layout.ComponentByteOffset(compIndex);
}
    
//------------------------------------------------------------------------------
inline MeshBuilder&
MeshBuilder::Vertex(uint32 vertexIndex, VertexAttr::Code attr, float x) {
    o_assert_dbg(this->inBegin);
    const int32 compIndex = this->Layout.ComponentIndexByVertexAttr(attr);
    uint8* ptr = this->vertexPointer + this->vertexByteOffset(vertexIndex, compIndex);
    VertexWriter::Write(ptr, this->Layout.Component(compIndex).Format, x);
    return *this;
}
    
//------------------------------------------------------------------------------
inline MeshBuilder&
MeshBuilder::Vertex(uint32 vertexIndex, VertexAttr::Code attr, float x, float y) {
    o_assert_dbg(this->inBegin);
    const int32 compIndex = this->Layout.ComponentIndexByVertexAttr(attr);
    uint8* ptr = this->vertexPointer + this->vertexByteOffset(vertexIndex, compIndex);
    VertexWriter::Write(ptr, this->Layout.Component(compIndex).Format, x, y);
    return *this;
}
    
//------------------------------------------------------------------------------
inline MeshBuilder&
MeshBuilder::Vertex(uint32 vertexIndex, VertexAttr::Code attr, float x, float y, float z) {
    o_assert_dbg(this->inBegin);
    const int32 compIndex = this->Layout.ComponentIndexByVertexAttr(attr);
    uint8* ptr = this->vertexPointer + this->vertexByteOffset(vertexIndex, compIndex);
    VertexWriter::Write(ptr, this->Layout.Component(compIndex).Format, x, y, z);
    return *this;
}

//------------------------------------------------------------------------------
inline MeshBuilder&
MeshBuilder::Vertex(uint32 vertexIndex, VertexAttr::Code attr, float x, float y, float z, float w) {
    o_assert_dbg(this->inBegin);
    const int32 compIndex = this->Layout.ComponentIndexByVertexAttr(attr);
    uint8* ptr = this->vertexPointer + this->vertexByteOffset(vertexIndex, compIndex);
    VertexWriter::Write(ptr, this->Layout.Component(compIndex).Format, x, y, z, w);
    return *this;
}

} // namespace Oryol
