//------------------------------------------------------------------------------
//  MeshBuilder.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "MeshBuilder.h"
#include "Core/Assertion.h"
#include <cstring>

namespace Oryol {

//------------------------------------------------------------------------------
MeshBuilder::MeshBuilder() :
NumVertices(0),
NumIndices(0),
IndicesType(IndexType::Index16),
VertexUsage(Usage::Immutable),
IndexUsage(Usage::Immutable),
inBegin(false),
resultValid(false),
vertexPointer(nullptr),
indexPointer(nullptr),
endPointer(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
void
MeshBuilder::Clear() {
    o_assert(!this->inBegin);
    
    this->NumVertices = 0;
    this->NumIndices = 0;
    this->IndicesType = IndexType::Index16;
    this->Layout.Clear();
    this->PrimitiveGroups.Clear();
    this->VertexUsage = Usage::Immutable;
    this->IndexUsage = Usage::Immutable;
    this->meshSetup = MeshSetup::FromStream();
    this->inBegin = false;
    this->resultValid = false;
    this->vertexPointer = nullptr;
    this->indexPointer = nullptr;
    this->endPointer = nullptr;
    this->stream.Invalidate();
}

//------------------------------------------------------------------------------
MeshBuilder&
MeshBuilder::Begin() {
    o_assert(!this->inBegin);
    o_assert(this->NumVertices > 0);
    o_assert(!this->Layout.Empty());
    o_assert(!this->PrimitiveGroups.Empty());
    this->inBegin = true;
    this->resultValid = false;

    // setup MeshSetup object
    this->meshSetup = MeshSetup::FromStream(this->VertexUsage, this->IndexUsage);
    this->meshSetup.Layout = this->Layout;
    this->meshSetup.NumVertices = this->NumVertices;
    this->meshSetup.NumIndices = this->NumIndices;
    this->meshSetup.IndicesType = this->IndicesType;
    for (const auto& primGroup : this->PrimitiveGroups) {
        this->meshSetup.AddPrimitiveGroup(primGroup);
    }
    
    // compute the required stream size
    const int32 vbSize  = Memory::RoundUp(this->NumVertices * this->Layout.ByteSize(), 4);
    const int32 ibSize  = this->NumIndices * IndexType::ByteSize(this->IndicesType);
    int32 allSize = vbSize + ibSize;
    this->meshSetup.StreamVertexOffset = 0;
    if (ibSize > 0) {
        this->meshSetup.StreamIndexOffset = vbSize;
    }
    
    // setup the memory stream object
    this->stream = MemoryStream::Create();
    this->stream->Open(OpenMode::WriteOnly);
    o_assert(this->stream->IsOpen());
    this->vertexPointer = this->stream->MapWrite(allSize);
    this->indexPointer  = this->vertexPointer + vbSize;
    this->endPointer    = this->indexPointer + ibSize;
    
    return *this;
}

//------------------------------------------------------------------------------
void
MeshBuilder::End() {
    o_assert(this->inBegin);
    o_assert(!this->resultValid);
    o_assert(this->stream->IsOpen());

    this->inBegin = false;
    this->resultValid = true;
    
    this->stream->UnmapWrite();
    this->stream->Close();
    
    this->vertexPointer = nullptr;
    this->indexPointer = nullptr;
    this->endPointer = nullptr;
}

//------------------------------------------------------------------------------
std::tuple<MeshSetup, Ptr<Stream>>
MeshBuilder::Result() const {
    return std::make_tuple(this->meshSetup, this->stream);
}

} // namespace Oryol
