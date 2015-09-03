#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::textureBundleBase
    @ingroup _priv
    @brief base class for textureBundle implementations
    
    Texture bundles wrap a combination of textures bound to the
    vertex and pixel shader stages.
    
    NOTE: later, this would also be the place where other shader resources
    would be defined (like static constant buffers, or general buffers),
    but this is not supported by the low-end 3D APIs, so we don't do this yet.
*/
#include "Resource/Core/resourceBase.h"
#include "Gfx/Setup/TextureBundleSetup.h"

namespace Oryol {
namespace _priv {
class textureBundleBase : public resourceBase<TextureBundleSetup> { };
} // namespace _priv
} // namespace Oryol
