//------------------------------------------------------------------------------
//  MeshBuilder.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "MeshBuilder.h"
#include "Core/Assertion.h"
#include "Gfx/Gfx.h"
#include <cstring>

namespace Oryol {

//------------------------------------------------------------------------------
MeshBuilder::Result
MeshBuilder::Build(std::function<void(MeshBuilder& mb)> func) {
    o_assert_dbg(!this->inBuild);
    o_assert_dbg(this->numVertices > 0);
    o_assert_dbg(!this->layout.Empty());
    this->inBuild = true;

    // compute the data buffer size
    const int vbSize  = this->numVertices * this->layout.ByteSize();
    const int ibSize  = this->numIndices * IndexType::ByteSize(this->indexType);
    int allSize = vbSize + ibSize;

    // setup the data buffer object
    this->vertexPointer = (uint8_t*) Memory::Alloc(allSize);
    this->indexPointer  = this->vertexPointer + vbSize;
    this->endPointer    = this->indexPointer + ibSize;

    // call the content-build function
    func(*this);

    // setup the Result object
    Result res;
    res.Data.MoveRaw(this->vertexPointer, allSize);
    res.Layout = this->layout;
    res.IndexType = this->indexType;
    res.VertexBufferDesc = BufferDesc()
        .Size(vbSize)
        .Type(BufferType::VertexBuffer)
        .Usage(this->vertexUsage)
        .Content(this->vertexPointer);
    if (ibSize > 0) {
        res.IndexBufferDesc = BufferDesc()
            .Size(ibSize)
            .Type(BufferType::IndexBuffer)
            .Usage(this->indexUsage)
            .Content(this->indexPointer);
    }
    else {
        res.IndexBufferDesc = BufferDesc();
    }
    this->inBuild = false;
    this->vertexPointer = nullptr;
    this->indexPointer = nullptr;
    this->endPointer = nullptr;
    return res;
}

} // namespace Oryol
