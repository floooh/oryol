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
    o_assert(!this->inBegin);
    o_assert(this->NumVertices > 0);
    o_assert(!this->Layout.Empty());
    o_assert(!this->PrimitiveGroups.Empty());
    this->inBegin = true;

    // setup MeshSetup object
    MeshSetup& meshSetup = this->setupAndData.Setup;
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
    
    // setup the data buffer object
    this->vertexPointer = this->setupAndData.Data.Add(allSize);
    this->indexPointer  = this->vertexPointer + vbSize;
    this->endPointer    = this->indexPointer + ibSize;
    
    return *this;
}

//------------------------------------------------------------------------------
SetupAndData<MeshSetup>
MeshBuilder::Build() {
    o_assert(this->inBegin);

    this->inBegin = false;

    // NOTE: explicit moves required by VS2013
    SetupAndData<MeshSetup> result(std::move(this->setupAndData));

    // clear private data, not configuration data
    this->vertexPointer = nullptr;
    this->indexPointer = nullptr;
    this->endPointer = nullptr;
    this->setupAndData.Setup = MeshSetup::FromData();
    this->setupAndData.Data.Clear();
    
    return result;
}

} // namespace Oryol
