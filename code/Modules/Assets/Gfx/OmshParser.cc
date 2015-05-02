//------------------------------------------------------------------------------
//  OmshParser.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "OmshParser.h"

namespace Oryol {

//------------------------------------------------------------------------------
bool
OmshParser::Parse(const void* ptr, uint32 size, MeshSetup& outSetup) {
    o_assert_dbg(ptr);
    o_assert_dbg(size > 4);
    o_assert_dbg(outSetup.NumPrimitiveGroups() == 0);
    o_assert_dbg(outSetup.Layout.Empty());

    // size must be multiple of 4
    if ((size & 3) != 0) {
        return false;
    }

    const uint32* u32StartPtr = (const uint32*) ptr;
    const uint32* u32Ptr = u32StartPtr;
    const uint32 u32Size = size >> 2;
    const uint32* u32EndPtr = u32StartPtr + u32Size;

    // check if enough data for header
    uint32 u32CheckSize = 7;
    if (u32CheckSize > u32Size) {
        return false;
    }

    // start parsing static header
    const uint32 magic = *u32Ptr++;
    if (magic != 'OMSH') {
        return false;
    }
    outSetup.NumVertices = *u32Ptr++;
    const uint32 vertexSize = *u32Ptr++;
    outSetup.NumIndices = *u32Ptr++;
    const uint32 indexSize = *u32Ptr++;
    if (2 == indexSize) {
        outSetup.IndicesType = IndexType::Index16;
    }
    else if (4 == indexSize) {
        outSetup.IndicesType = IndexType::Index32;
    }
    else {
        return false;
    }
    const uint32 numVertexAttrs = *u32Ptr++;
    if (numVertexAttrs > VertexAttr::NumVertexAttrs) {
        return false;
    }
    const uint32 numPrimGroups = *u32Ptr++;
    if (numPrimGroups > MeshSetup::MaxNumPrimGroups) {
        return false;
    }

    // check if enough data for vertex components
    u32CheckSize += numVertexAttrs * 2;
    if (u32CheckSize > u32Size) {
        return false;
    }
    for (uint32 i = 0; i < numVertexAttrs; i++) {
        VertexComponent comp;
        comp.Attr = (VertexAttr::Code) *u32Ptr++;
        comp.Format = (VertexFormat::Code) *u32Ptr++;
        outSetup.Layout.Add(comp);
    }

    // check if enough data for primitive groups
    u32CheckSize += numPrimGroups * 3;
    if (u32CheckSize > u32Size) {
        return false;
    }
    for(uint32 i = 0; i < numPrimGroups; i++) {
        PrimitiveGroup primGroup;
        primGroup.PrimType = (PrimitiveType::Code) *u32Ptr++;
        primGroup.BaseElement = *u32Ptr++;
        primGroup.NumElements = *u32Ptr++;
        outSetup.AddPrimitiveGroup(primGroup);
    }

    // check if enough data for vertices
    const uint32 u32VertexDataSize = (outSetup.NumVertices * vertexSize) >> 2;
    u32CheckSize += u32VertexDataSize;
    if (u32CheckSize > u32Size) {
        return false;
    }
    outSetup.DataVertexOffset = (u32Ptr - u32StartPtr) << 2;
    u32Ptr += u32VertexDataSize;

    // check if enough data for indices
    const uint32 u32IndexDataSize = (outSetup.NumIndices * indexSize) >> 2;
    u32CheckSize += u32IndexDataSize;
    if (u32CheckSize > u32Size) {
        return false;
    }
    outSetup.DataIndexOffset = (u32Ptr - u32StartPtr) << 2;
    u32Ptr += u32IndexDataSize;

    // cleanly reached the end?
    return (u32EndPtr == u32Ptr);
}

} // namespace Oryol
