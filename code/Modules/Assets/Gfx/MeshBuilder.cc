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
    this->inBegin = false;
    this->resultValid = false;
    this->vertexPointer = nullptr;
    this->indexPointer = nullptr;
    this->endPointer = nullptr;
    this->setupAndStream.Setup = MeshSetup::FromData();
    this->setupAndStream.Stream.invalidate();
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
    MeshSetup& meshSetup = this->setupAndStream.Setup;
    meshSetup = MeshSetup::FromData(this->VertexUsage, this->IndexUsage);
    meshSetup.Layout = this->Layout;
    meshSetup.NumVertices = this->NumVertices;
    meshSetup.NumIndices = this->NumIndices;
    meshSetup.IndicesType = this->IndicesType;
    for (const auto& primGroup : this->PrimitiveGroups) {
        meshSetup.AddPrimitiveGroup(primGroup);
    }
    
    // compute the required stream size
    const int32 vbSize  = Memory::RoundUp(this->NumVertices * this->Layout.ByteSize(), 4);
    const int32 ibSize  = this->NumIndices * IndexType::ByteSize(this->IndicesType);
    int32 allSize = vbSize + ibSize;
    meshSetup.DataVertexOffset = 0;
    if (ibSize > 0) {
        meshSetup.DataIndexOffset = vbSize;
    }
    
    // setup the memory stream object
    this->setupAndStream.Stream = MemoryStream::Create();
    this->setupAndStream.Stream->Open(OpenMode::WriteOnly);
    o_assert(this->setupAndStream.Stream->IsOpen());
    this->vertexPointer = this->setupAndStream.Stream->MapWrite(allSize);
    this->indexPointer  = this->vertexPointer + vbSize;
    this->endPointer    = this->indexPointer + ibSize;
    
    return *this;
}

//------------------------------------------------------------------------------
void
MeshBuilder::End() {
    o_assert(this->inBegin);
    o_assert(!this->resultValid);
    o_assert(this->setupAndStream.Stream->IsOpen());

    this->inBegin = false;
    this->resultValid = true;
    
    this->setupAndStream.Stream->UnmapWrite();
    this->setupAndStream.Stream->Close();
    
    this->vertexPointer = nullptr;
    this->indexPointer = nullptr;
    this->endPointer = nullptr;
}

//------------------------------------------------------------------------------
const SetupAndStream<MeshSetup>&
MeshBuilder::Result() const {
    return this->setupAndStream;
}

} // namespace Oryol
