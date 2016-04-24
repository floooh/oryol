#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlMesh
    @ingroup _priv
    @brief Metal implementation of class mesh
*/
#include "Core/Containers/StaticArray.h"
#include "Gfx/resource/meshBase.h"
#include "Gfx/mtl/mtl_decl.h"
#include "Gfx/Core/GfxConfig.h"

namespace Oryol {
namespace _priv {

class mtlMesh : public meshBase {
public:
    /// destructor
    ~mtlMesh();

    /// clear the object (called from meshFactory::DestroyResource())
    void Clear();

    static const int NumSlots = GfxConfig::MtlMaxInflightFrames;
    struct buffer {
        buffer();
        int updateFrameIndex;
        uint8_t numSlots;
        uint8_t activeSlot;
        StaticArray<ORYOL_OBJC_TYPED_ID(MTLBuffer), NumSlots> mtlBuffers;
    };
    // indices into buffers array (first entry is vertex buffers, second entry is index buffers
    static const int vb = 0;
    static const int ib = 1;
    StaticArray<buffer,2> buffers;
};

} // namespace _priv
} // namespace Oryol
