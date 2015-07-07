#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlDrawStateFactory
    @ingroup _priv
    @brief Metal implementation of drawStateFactory
*/
#include "Gfx/Resource/drawStateFactoryBase.h"
#include "Gfx/mtl/mtl_decl.h"

namespace Oryol {
namespace _priv {

class mtlDrawStateFactory : public drawStateFactoryBase {
public:
    /// setup a new drawState resource
    ResourceState::Code SetupResource(drawState& ds);
    /// destroy drawState resource
    void DestroyResource(drawState& ds);
};

} // namespace _priv
} // namespace Oryol
