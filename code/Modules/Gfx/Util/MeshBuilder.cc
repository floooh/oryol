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
inBegin(false),
resultValid(false),
header(nullptr),
headerPointer(nullptr),
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
    this->primGroups.Clear();
    this->meshSetup = MeshSetup();
    this->inBegin = false;
    this->resultValid = false;
    this->header = nullptr;
    this->headerPointer = nullptr;
    this->vertexPointer = nullptr;
    this->indexPointer = nullptr;
    this->endPointer = nullptr;
    this->stream.Invalidate();
    this->workSet.layout.Clear();
}

//------------------------------------------------------------------------------
void
MeshBuilder::AddPrimitiveGroup(const PrimitiveGroup& primGroup) {
    o_assert(!this->inBegin);
    this->primGroups.Add(primGroup);
}

//------------------------------------------------------------------------------
void
MeshBuilder::AddPrimitiveGroup(PrimitiveType::Code type, int32 baseElement, int32 numElements) {
    o_assert(!this->inBegin);
    this->primGroups.Emplace(type, baseElement, numElements);
}

//------------------------------------------------------------------------------
MeshBuilder&
MeshBuilder::Begin() {
    o_assert(!this->inBegin);
    o_assert(this->NumVertices > 0);
    o_assert(!this->Layout.Empty());
    o_assert(!this->primGroups.Empty());
    this->inBegin = true;
    this->resultValid = false;
    
    // copy config objects into internal temporary state
    this->workSet.layout = this->Layout;
    
    // compute the required stream size
    int32 hdrSize = sizeof(Header);
    hdrSize      += sizeof(HeaderVertexComponent) * this->workSet.layout.NumComponents();
    hdrSize      += sizeof(HeaderPrimitiveGroup) * this->primGroups.Size();
    int32 vbSize  = Memory::RoundUp(this->NumVertices * this->workSet.layout.ByteSize(), 4);
    int32 ibSize  = this->NumIndices * IndexType::ByteSize(this->IndicesType);
    int32 allSize = hdrSize + vbSize + ibSize;
    
    // setup the memory stream object
    this->stream = MemoryStream::Create();
    this->stream->Open(OpenMode::WriteOnly);
    o_assert(this->stream->IsOpen());
    this->headerPointer = this->stream->MapWrite(allSize);
    this->vertexPointer = this->headerPointer + hdrSize;
    this->indexPointer  = this->vertexPointer + vbSize;
    this->endPointer    = this->indexPointer + ibSize;
    
    // write the header
    this->header = (Header*) this->headerPointer;
    this->header->magic = 'ORAW';
    this->header->numVertices = this->NumVertices;
    this->header->numIndices  = this->NumIndices;
    this->header->indexType   = (int32) this->IndicesType;
    this->header->numVertexComponents = this->workSet.layout.NumComponents();
    this->header->numPrimitiveGroups  = this->primGroups.Size();
    this->header->verticesByteSize = vbSize;
    this->header->indicesByteSize  = ibSize;
    
    HeaderVertexComponent* hdrComp = (HeaderVertexComponent*) (this->headerPointer + sizeof(Header));
    for (int32 i = 0; i < this->workSet.layout.NumComponents(); i++, hdrComp++) {
        const VertexComponent& src = this->workSet.layout.Component(i);
        hdrComp->attr   = src.Attr;
        hdrComp->format = src.Format;
    }
    
    HeaderPrimitiveGroup* hdrPrimGroup = (HeaderPrimitiveGroup*) hdrComp;
    for (int32 i = 0; i < this->primGroups.Size(); i++, hdrPrimGroup++) {
        const PrimitiveGroup& src = this->primGroups[i];
        hdrPrimGroup->type = (uint32) src.PrimType;
        hdrPrimGroup->baseElement = src.BaseElement;
        hdrPrimGroup->numElements = src.NumElements;
    }
    o_assert((uint8*)hdrPrimGroup == this->vertexPointer);
    
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
    
    this->header = nullptr;
    this->headerPointer = nullptr;
    this->vertexPointer = nullptr;
    this->indexPointer = nullptr;
    this->endPointer = nullptr;
}

//------------------------------------------------------------------------------
const Ptr<Stream>&
MeshBuilder::GetStream() const {
    return this->stream;
}

} // namespace Oryol
