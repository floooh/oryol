#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlMesh
    @ingroup _priv
    @brief Metal implementation of class mesh
*/
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

    ORYOL_OBJC_TYPED_ID(MTLBuffer) mtlVertexBuffer;
    ORYOL_OBJC_TYPED_ID(MTLBuffer) mtlIndexBuffer;
};

} // namespace _priv
} // namespace Oryol
