#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::MeshBuilder
    @brief build mesh data programmatically
    
    The MeshBuilder class simplifies creating mesh data from scratch with
    the CPU. It is mainly build for convenience, less for performance, 
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
    RenderFacade::CreateMesh().
    
    Vertex format packing happens on the fly when writing vertex data 
    according to the vertex layout given.
    
    This is the format of the stream data that will be written, this
    is compatible with the RawMeshLoader format:
    
    uint32 magic = 'ORAW'
    uint32 numVertices
    uint32 numIndices
    uint32 indexType (0=no indices, 1=16-bit, 2=32-bit)
    uint32 numVertexComponents
    uint32 numPrimitiveGroups
    uint32 verticesByteSize (size of vertex data in bytes, rounded up to 4)
    uint32 indicesByteSize (size of index data in bytes)
    [1..numVertexComponents]:
        uint32 attr
        uint32 format       (VertexFormat::Code)
    [1..numPrimitiveGroups]
        uint32 type         (PrimitiveType::Code)
        uint32 baseElement
        uint32 numElements
    [1..numVertices]
        [1..N bytes per vertex]
    [4-byte aligned, 1..numIndices]
        [2 or 4 bytes per index]
 
    @see VertexWriter, ShapeBuilder
*/
#include "Core/Types.h"
#include "Render/Core/Enums.h"
#include "Render/Core/VertexLayout.h"
#include "Render/Core/PrimitiveGroup.h"
#include "Render/Setup/MeshSetup.h"
#include "Render/Util/VertexWriter.h"
#include "IO/Stream/MemoryStream.h"

namespace Oryol {
namespace Render {

class MeshBuilder {
public:
    /// constructor
    MeshBuilder();
    
    /// set number of vertices
    void SetNumVertices(uint32 numVertices);
    /// set number of indices (default 0 for non-indexed meshes)
    void SetNumIndices(uint32 numIndices);
    /// set the index type (default is 16-bit indices)
    void SetIndexType(IndexType::Code indexType);
    /// read/write access to vertex layout
    class VertexLayout& VertexLayout();
    /// read-only access to vertex layout
    const class VertexLayout& VertexLayout() const;
    /// add a primitive group (at least one needed!)
    void AddPrimitiveGroup(const PrimitiveGroup& primGroup);
    /// add a primitive group (at least one needed!)
    void AddPrimitiveGroup(PrimitiveType::Code type, int32 baseElement, int32 numElements);
    
    /// begin writing vertex and index data
    void Begin();
    /// write D component vertex data
    void Vertex(uint32 vertexIndex, VertexAttr::Code attr, float32 x);
    /// write 2D vertex data
    void Vertex(uint32 vertexIndex, VertexAttr::Code attr, float32 x, float32 y);
    /// write 3D vertex data
    void Vertex(uint32 vertexIndex, VertexAttr::Code attr, float32 x, float32 y, float32 z);
    /// write 4D vertex data
    void Vertex(uint32 vertexIndex, VertexAttr::Code attr, float32 x, float32 y, float32 z, float32 w);
    /// write 16-bit vertex-index at index-buffer-index
    void Index(uint32 index, uint16 vertexIndex);
    /// write 32-bit vertex-index at index-buffer-index
    void Index32(uint32 index, uint32 vertexIndex);
    /// write 16-bit triangle indices
    void Triangle(uint32 triangleIndex, uint16 vertexIndex0, uint16 vertexIndex1, uint16 vertexIndex2);
    /// write 32-bit triangle indices
    void Triangle32(uint32 triangleIndex, uint32 vertexIndex0, uint32 vertexIndex1, uint32 vertexIndex2);
    /// end writing vertex and index data
    void End();
    
    /// clear the mesh builder
    void Clear();
    /// get the resulting data stream with vertex and index data
    const Core::Ptr<IO::Stream>& GetStream() const;
    
    /// the mesh data header
    struct Header {
        uint32 magic;
        uint32 numVertices;
        uint32 numIndices;
        uint32 indexType;
        uint32 numVertexComponents;
        uint32 numPrimitiveGroups;
        uint32 verticesByteSize;        // rounded up to multiples of 4!
        uint32 indicesByteSize;
    };
    
    /// a vertex component entry in the header
    struct HeaderVertexComponent {
        uint32 attr;
        uint32 format;
    };
    
    /// a primitive group entry in the header
    struct HeaderPrimitiveGroup {
        uint32 type;
        uint32 baseElement;
        uint32 numElements;
    };
    
private:
    /// compute byte offset into vertex buffer given vertex and component index
    uint32 vertexByteOffset(uint32 vertexIndex, int32 compIndex) const;

    uint32 numVertices;
    uint32 numIndices;
    IndexType::Code indexType;
    class VertexLayout layout;
    Core::Array<PrimitiveGroup> primGroups;
    Core::Ptr<IO::MemoryStream> stream;
    MeshSetup meshSetup;
    bool inBegin;
    bool resultValid;
    
    uint8* headerPointer;
    uint8* vertexPointer;
    uint8* indexPointer;
    uint8* endPointer;
};

//------------------------------------------------------------------------------
inline class VertexLayout&
MeshBuilder::VertexLayout() {
    return this->layout;
}

//------------------------------------------------------------------------------
inline const class VertexLayout&
MeshBuilder::VertexLayout() const {
    return this->layout;
}

//------------------------------------------------------------------------------
inline void
MeshBuilder::Index(uint32 index, uint16 vertexIndex) {
    o_assert_dbg(this->inBegin && (index < this->numIndices) && (this->indexType == IndexType::Index16));
    o_assert_dbg(vertexIndex < this->numVertices);
    
    uint16* ptr = ((uint16*)this->indexPointer) + index;
    o_assert_dbg(ptr < (uint16*)this->endPointer);
    *ptr = vertexIndex;
}
    
//------------------------------------------------------------------------------
inline void
MeshBuilder::Index32(uint32 index, uint32 vertexIndex) {
    o_assert_dbg(this->inBegin && (index < this->numIndices) && (this->indexType == IndexType::Index32));
    o_assert_dbg(vertexIndex < this->numVertices);
    
    uint32* ptr = ((uint32*)this->indexPointer) + index;
    o_assert_dbg(ptr < (uint32*)this->endPointer);
    *ptr = vertexIndex;
}

//------------------------------------------------------------------------------
inline void
MeshBuilder::Triangle(uint32 triIndex, uint16 vi0, uint16 vi1, uint16 vi2) {
    o_assert_dbg(this->inBegin && (triIndex*3 < this->numIndices));
    o_assert_dbg((vi0 < this->numVertices) && (vi1 < this->numVertices) && (vi2 < this->numVertices));
    
    uint16* ptr = ((uint16*)this->indexPointer) + triIndex * 3;
    o_assert_dbg(ptr < (uint16*)this->endPointer);
    ptr[0] = vi0; ptr[1] = vi1; ptr[2] = vi2;
}


//------------------------------------------------------------------------------
inline uint32
MeshBuilder::vertexByteOffset(uint32 vertexIndex, int32 compIndex) const {
    o_assert_dbg(vertexIndex < this->numVertices);
    o_assert_dbg(InvalidIndex != compIndex);
    return vertexIndex * this->layout.ByteSize() + this->layout.ComponentByteOffset(compIndex);
}
    
//------------------------------------------------------------------------------
inline void
MeshBuilder::Vertex(uint32 vertexIndex, VertexAttr::Code attr, float x) {
    o_assert_dbg(this->inBegin);
    const int32 compIndex = this->layout.ComponentIndexByVertexAttr(attr);
    uint8* ptr = this->vertexPointer + this->vertexByteOffset(vertexIndex, compIndex);
    VertexWriter::Write(ptr, this->layout.Component(compIndex).Format(), x);
}
    
//------------------------------------------------------------------------------
inline void
MeshBuilder::Vertex(uint32 vertexIndex, VertexAttr::Code attr, float x, float y) {
    o_assert_dbg(this->inBegin);
    const int32 compIndex = this->layout.ComponentIndexByVertexAttr(attr);
    uint8* ptr = this->vertexPointer + this->vertexByteOffset(vertexIndex, compIndex);
    VertexWriter::Write(ptr, this->layout.Component(compIndex).Format(), x, y);
}
    
//------------------------------------------------------------------------------
inline void
MeshBuilder::Vertex(uint32 vertexIndex, VertexAttr::Code attr, float x, float y, float z) {
    o_assert_dbg(this->inBegin);
    const int32 compIndex = this->layout.ComponentIndexByVertexAttr(attr);
    uint8* ptr = this->vertexPointer + this->vertexByteOffset(vertexIndex, compIndex);
    VertexWriter::Write(ptr, this->layout.Component(compIndex).Format(), x, y, z);
}

//------------------------------------------------------------------------------
inline void
MeshBuilder::Vertex(uint32 vertexIndex, VertexAttr::Code attr, float x, float y, float z, float w) {
    o_assert_dbg(this->inBegin);
    const int32 compIndex = this->layout.ComponentIndexByVertexAttr(attr);
    uint8* ptr = this->vertexPointer + this->vertexByteOffset(vertexIndex, compIndex);
    VertexWriter::Write(ptr, this->layout.Component(compIndex).Format(), x, y, z, w);
}

} // namespace Render
} // namespace Oryol