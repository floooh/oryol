#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::textureBase
    @ingroup _priv
    @brief private: base class for platform-specific texture implementation
*/
#include "Resource/Core/resourceBase.h"
#include "Gfx/Attrs/TextureAttrs.h"
#include "Gfx/Setup/TextureSetup.h"
#include "Gfx/Core/Enums.h"

namespace Oryol {
namespace _priv {

class textureBase : public resourceBase<TextureSetup> {
public:
    /// constructor
    textureBase();
    
    /// texture attributes
    TextureAttrs textureAttrs;
    
    /// clear the object
    void Clear();
};

} // namespace _priv
} // namespace Oryol

    