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

namespace Oryol {
namespace _priv {

class mtlMesh : public meshBase {
public:
    /// constructor
    mtlMesh();
    /// destructor
    ~mtlMesh();

    /// clear the object (called from meshFactory::DestroyResource())
    void Clear();

    /// max number of vertex buffer slots (used for double-buffering dynamic vertex data)
    static const int32 NumSlots = 2;
    /// active vertex buffer slot
    uint8 activeVertexBufferSlot;

    StaticArray<ORYOL_OBJC_TYPED_ID(MTLBuffer),NumSlots> mtlVertexBuffers;
    ORYOL_OBJC_TYPED_ID(MTLBuffer) mtlIndexBuffer;
};

} // namespace _priv
} // namespace Oryol
