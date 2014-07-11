//------------------------------------------------------------------------------
//  MeshBuilder.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "MeshBuilder.h"
#include "Core/Assert.h"
#include <cstring>

namespace Oryol {
namespace Render {

using namespace Core;
using namespace IO;

//------------------------------------------------------------------------------
MeshBuilder::MeshBuilder() :
numVertices(0),
numIndices(0),
indexType(IndexType::Index16),
inBegin(false),
resultValid(false),
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
    
    this->numVertices = 0;
    this->numIndices = 0;
    this->indexType = IndexType::Index16;
    this->layout.Clear();
    this->primGroups.Clear();
    this->meshSetup = MeshSetup();
    this->inBegin = false;
    this->resultValid = false;
    this->headerPointer = nullptr;
    this->vertexPointer = nullptr;
    this->indexPointer = nullptr;
    this->endPointer = nullptr;
    this->stream.Invalidate();
}

//------------------------------------------------------------------------------
void
MeshBuilder::SetNumVertices(uint32 num) {
    o_assert(num > 0);
    o_assert(!this->inBegin);
    this->numVertices = num;
}

//------------------------------------------------------------------------------
void
MeshBuilder::SetNumIndices(uint32 num) {
    o_assert(!this->inBegin);
    this->numIndices = num;
}

//------------------------------------------------------------------------------
void
MeshBuilder::SetIndexType(IndexType::Code t) {
    o_assert(!this->inBegin);
    this->indexType = t;
}

//------------------------------------------------------------------------------
void
MeshBuilder::AddPrimitiveGroup(const PrimitiveGroup& primGroup) {
    o_assert(!this->inBegin);
    this->primGroups.AddBack(primGroup);
}

//------------------------------------------------------------------------------
void
MeshBuilder::AddPrimitiveGroup(PrimitiveType::Code type, int32 baseElement, int32 numElements) {
    o_assert(!this->inBegin);
    this->primGroups.EmplaceBack(type, baseElement, numElements);
}

//------------------------------------------------------------------------------
void
MeshBuilder::Begin() {
    o_assert(!this->inBegin);
    o_assert(this->numVertices > 0);
    o_assert(!this->layout.Empty());
    o_assert(!this->primGroups.Empty());
    this->inBegin = true;
    this->resultValid = false;
    
    // compute the required stream size
    int32 hdrSize = sizeof(Header);
    hdrSize      += sizeof(HeaderVertexComponent) * this->layout.NumComponents();
    hdrSize      += sizeof(HeaderPrimitiveGroup) * this->primGroups.Size();
    int32 vbSize  = Memory::RoundUp(this->numVertices * this->layout.ByteSize(), 4);
    int32 ibSize  = this->numIndices * IndexType::ByteSize(this->indexType);
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
    Header* header = (Header*) this->headerPointer;
    header->magic = 'ORAW';
    header->numVertices = this->numVertices;
    header->numIndices  = this->numIndices;
    header->indexType   = (int32) this->indexType;
    header->numVertexComponents = this->layout.NumComponents();
    header->numPrimitiveGroups  = this->primGroups.Size();
    header->verticesByteSize = vbSize;
    header->indicesByteSize  = ibSize;
    
    HeaderVertexComponent* hdrComp = (HeaderVertexComponent*) (this->headerPointer + sizeof(Header));
    for (int32 i = 0; i < this->layout.NumComponents(); i++, hdrComp++) {
        const VertexComponent& src = this->layout.Component(i);
        hdrComp->attr   = src.Attr();
        hdrComp->format = src.Format();
    }
    
    HeaderPrimitiveGroup* hdrPrimGroup = (HeaderPrimitiveGroup*) hdrComp;
    for (int32 i = 0; i < this->primGroups.Size(); i++, hdrPrimGroup++) {
        const PrimitiveGroup& src = this->primGroups[i];
        hdrPrimGroup->type = (uint32) src.GetPrimitiveType();
        hdrPrimGroup->baseElement = src.GetBaseElement();
        hdrPrimGroup->numElements = src.GetNumElements();
    }
    o_assert((uint8*)hdrPrimGroup == this->vertexPointer);
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

} // namespace Render
} // namespace Oryol
