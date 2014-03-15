//------------------------------------------------------------------------------
//  MeshBuilder.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "MeshBuilder.h"
#include "Core/Macros.h"

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
MeshBuilder::SetNumVertices(int32 num) {
    o_assert(num > 0);
    o_assert(!this->inBegin);
    this->numVertices = num;
}

//------------------------------------------------------------------------------
void
MeshBuilder::SetNumIndices(int32 num) {
    o_assert(num >= 0);
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
MeshBuilder::AddComponent(const VertexComponent& comp) {
    o_assert(!this->inBegin);
    this->layout.Add(comp);
}

//------------------------------------------------------------------------------
void
MeshBuilder::AddPrimGroup(const PrimitiveGroup& primGroup) {
    o_assert(!this->inBegin);
    this->primGroups.AddBack(primGroup);
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
    hdrSize      += sizeof(HeaderVertexComponent) * this->layout.GetNumComponents();
    hdrSize      += sizeof(HeaderPrimitiveGroup) * this->primGroups.Size();
    int32 vbSize  = Memory::RoundUp(this->numVertices * this->layout.GetByteSize(), 4);
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
    header->numVertexComponents = this->layout.GetNumComponents();
    header->numPrimitiveGroups  = this->primGroups.Size();
    header->verticesByteSize = vbSize;
    header->indicesByteSize  = ibSize;
    
    HeaderVertexComponent* hdrComp = (HeaderVertexComponent*) (this->headerPointer + sizeof(Header));
    for (int32 i = 0; i < this->layout.GetNumComponents(); i++, hdrComp++) {
        const VertexComponent& src = this->layout.GetComponent(i);
        hdrComp->format = src.GetFormat();
        Memory::Clear(hdrComp->attrName, sizeof(hdrComp->attrName));
        std::strncpy(hdrComp->attrName, src.GetAttrName().AsCStr(), sizeof(hdrComp->attrName) - 1);
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

} // namespace Render
} // namespace Oryol