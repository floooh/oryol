#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::OmshParser
    @ingroup Assets
    @brief in-memory OMSH mesh file-format parser
    
    Takes a piece of memory with OMSH data in it, and returns
    a MeshSetup object. OMSH mesh data is created by the 
    oryol-export tool ( https://github.com/floooh/oryol-tools )
    
    OMSH file format (see oryol-tools project):
    
    struct {
        uint32 magic = 'OMSH';
        uint32 numVertices;
        uint32 vertexSize;      // size of one vertex in bytes (guaranteed multiple of 4)
        uint32 numIndices;
        uint32 indexSize;       // size of one index, must be 2 or 4
        uint32 numVertexAttrs;
        uint32 numPrimitiveGroups;
        struct {
            uint32 attr;        // values of VertexAttr::Code
            uint32 format;      // values of VertexFormat::Code
        } vertexAttrs[numVertexAttr];
        struct {
            uint32 primitiveType;   // values of OmshPrimitiveType (see below)
            uint32 baseElement;
            uint32 numElements;
        } primitiveGroups[numPrimitiveGroups];
        uint8 vertexData[numVertices * vertexSize];
        uint8 indexData[numIndices * indexSize];
        - optional: 2 zero-bytes of padding if odd number of 16-bit-indices
    };

    enum OmshPrimitiveType {
        Points = 0,
        Lines = 1,
        LineLoop = 2,
        LineStrip = 3,
        Triangles = 4,
        TriangleStrip = 5,
        TriangleFan = 6,
    }
*/
#include "Gfx/GfxTypes.h"

namespace Oryol {

class OmshParser {
public:
    /// parse block of memory into MeshSetup object
    static bool Parse(const void* ptr, uint32_t size, MeshSetup& outSetup);
private:
    /// translate OMSH primtype to actual primtype
    static PrimitiveType::Code translatePrimType(uint32_t omshPrimType);
};

} // namespace Oryol
