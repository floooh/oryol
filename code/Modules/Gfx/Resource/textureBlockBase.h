#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::textureBlockBase
    @ingroup _priv
    @brief base class for textureBlock implementations
    
    Texture blocks wrap a combination of textures that are bound
    to one shader stage with a specific update frequency.
*/
#include "Resource/Core/resourceBase.h"
#include "Gfx/Setup/TextureBlockSetup.h"

namespace Oryol {
namespace _priv {
class textureBlockBase : public resourceBase<TextureBlockSetup> { };
} // namespace _priv
} // namespace Oryol
