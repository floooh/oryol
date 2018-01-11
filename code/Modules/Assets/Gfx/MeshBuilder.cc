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
    this->result.VertexBufferSetup = BufferSetup::Make(vbSize, BufferType::VertexBuffer, this->VertexUsage);
    if (ibSize > 0) {
        this->result.IndexBufferSetup = BufferSetup::Make(ibSize, BufferType::IndexBuffer, this->IndexUsage);
        this->result.IndexBufferSetup.Offset = this->result.VertexBufferSetup.Size;
    }
    else {
        this->result.IndexBufferSetup = BufferSetup();
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
    this->result.VertexBufferSetup = BufferSetup();
    this->result.IndexBufferSetup = BufferSetup();
    this->result.Layout = VertexLayout();
    this->result.IndexType = IndexType::Invalid;
    this->result.Data.Clear();
    
    return res;
}

} // namespace Oryol
