//------------------------------------------------------------------------------
//  MeshBuilder.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "MeshBuilder.h"
#include "Core/Assertion.h"
#include <cstring>

namespace Oryol {

//------------------------------------------------------------------------------
MeshBuilder&
MeshBuilder::Begin() {
    o_assert_dbg(!this->inBegin);
    o_assert_dbg(this->NumVertices > 0);
    o_assert_dbg(!this->Layout.Empty());
    this->inBegin = true;

    // compute the data buffer size
    const int vbSize  = this->NumVertices * this->Layout.ByteSize();
    const int ibSize  = this->NumIndices * IndexType::ByteSize(this->IndexType);
    int allSize = vbSize + ibSize;

    // setup the data buffer object
    this->vertexPointer = this->result.Data.Add(allSize);
    this->indexPointer  = this->vertexPointer + vbSize;
    this->endPointer    = this->indexPointer + ibSize;

    // setup Result object
    this->result.Layout = this->Layout;
    this->result.IndexType = this->IndexType;
    this->result.VertexBufferDesc = MakeBufferDesc()
        .Size(vbSize)
        .Type(BufferType::VertexBuffer)
        .Usage(this->VertexUsage);
    if (ibSize > 0) {
        this->result.IndexBufferDesc = MakeBufferDesc()
            .Size(ibSize)
            .Type(BufferType::IndexBuffer)
            .Usage(this->IndexUsage)
            .Offset(this->result.VertexBufferDesc.Size);
    }
    else {
        this->result.IndexBufferDesc = BufferDesc();
    }
    return *this;
}

//------------------------------------------------------------------------------
MeshBuilder::Result
MeshBuilder::Build() {
    o_assert(this->inBegin);

    this->inBegin = false;

    // NOTE: explicit moves required by VS2013
    Result res(std::move(this->result));

    // clear private data, not configuration data
    this->vertexPointer = nullptr;
    this->indexPointer = nullptr;
    this->endPointer = nullptr;
    this->result.VertexBufferDesc = BufferDesc();
    this->result.IndexBufferDesc = BufferDesc();
    this->result.Layout = VertexLayout();
    this->result.IndexType = IndexType::Invalid;
    this->result.Data.Clear();
    
    return res;
}

} // namespace Oryol
