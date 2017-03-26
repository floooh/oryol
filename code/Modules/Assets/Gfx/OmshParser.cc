//------------------------------------------------------------------------------
//  OmshParser.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "OmshParser.h"

namespace Oryol {

//------------------------------------------------------------------------------
PrimitiveType::Code
OmshParser::translatePrimType(uint32_t omshPrimType) {
    switch (omshPrimType) {
        case 0:     return PrimitiveType::Points;
        case 1:     return PrimitiveType::Lines;
        case 3:     return PrimitiveType::LineStrip;
        case 4:     return PrimitiveType::Triangles;
        case 5:     return PrimitiveType::TriangleStrip;
        default:    return PrimitiveType::InvalidPrimitiveType;
    }
}

//------------------------------------------------------------------------------
bool
OmshParser::Parse(const void* ptr, uint32_t size, MeshSetup& outSetup) {
    o_assert_dbg(ptr);
    o_assert_dbg(size > 4);
    o_assert_dbg(outSetup.NumPrimitiveGroups() == 0);
    o_assert_dbg(outSetup.Layout.Empty());

    // size must be multiple of 4
    if ((size & 3) != 0) {
        return false;
    }

    const uint32_t* u32StartPtr = (const uint32_t*) ptr;
    const uint32_t* u32Ptr = u32StartPtr;
    const uint32_t u32Size = size >> 2;
    const uint32_t* u32EndPtr = u32StartPtr + u32Size;

    // check if enough data for header
    uint32_t u32CheckSize = 7;
    if (u32CheckSize > u32Size) {
        return false;
    }

    // start parsing static header
    const uint32_t magic = *u32Ptr++;
    if (magic != 'OMSH') {
        return false;
    }
    outSetup.NumVertices = *u32Ptr++;
    const uint32_t vertexSize = *u32Ptr++;
    outSetup.NumIndices = *u32Ptr++;
    const uint32_t indexSize = *u32Ptr++;
    if (2 == indexSize) {
        outSetup.IndicesType = IndexType::Index16;
    }
    else if (4 == indexSize) {
        outSetup.IndicesType = IndexType::Index32;
    }
    else {
        return false;
    }
    const uint32_t numVertexAttrs = *u32Ptr++;
    if (numVertexAttrs > VertexAttr::NumVertexAttrs) {
        return false;
    }
    const uint32_t numPrimGroups = *u32Ptr++;
    if (numPrimGroups > GfxConfig::MaxNumPrimGroups) {
        return false;
    }

    // check if enough data for vertex components
    u32CheckSize += numVertexAttrs * 2;
    if (u32CheckSize > u32Size) {
        return false;
    }
    for (uint32_t i = 0; i < numVertexAttrs; i++) {
        VertexLayout::Component comp;
        comp.Attr = (VertexAttr::Code) *u32Ptr++;
        comp.Format = (VertexFormat::Code) *u32Ptr++;
        outSetup.Layout.Add(comp);
    }

    // check if enough data for primitive groups
    u32CheckSize += numPrimGroups * 3;
    if (u32CheckSize > u32Size) {
        return false;
    }
    for(uint32_t i = 0; i < numPrimGroups; i++) {
        PrimitiveGroup primGroup;
        // skip primitive type
        u32Ptr++;
        primGroup.BaseElement = *u32Ptr++;
        primGroup.NumElements = *u32Ptr++;
        outSetup.AddPrimitiveGroup(primGroup);
    }

    // check if enough data for vertices
    const uint32_t u32VertexDataSize = (outSetup.NumVertices * vertexSize) >> 2;
    u32CheckSize += u32VertexDataSize;
    if (u32CheckSize > u32Size) {
        return false;
    }
    outSetup.VertexDataOffset = int(u32Ptr - u32StartPtr) << 2;
    u32Ptr += u32VertexDataSize;

    // check if enough data for indices (index block is padded
    // so that size is multiple of 4)
    uint32_t indexDataSize = outSetup.NumIndices * indexSize;
    if ((indexDataSize & 3) != 0) {
        indexDataSize += 2;
        o_assert_dbg((indexDataSize & 3) == 0);
    }
    const uint32_t u32IndexDataSize = indexDataSize >> 2;
    u32CheckSize += u32IndexDataSize;
    if (u32CheckSize > u32Size) {
        return false;
    }
    outSetup.IndexDataOffset = int(u32Ptr - u32StartPtr) << 2;
    u32Ptr += u32IndexDataSize;

    // cleanly reached the end?
    return (u32EndPtr == u32Ptr);
}

} // namespace Oryol
