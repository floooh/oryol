#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::TextureLoader
    @ingroup Assets
    @brief standard texture loader for most block-compressed texture file formats
*/
#include "Gfx/GfxTypes.h"

namespace Oryol {

class TextureLoader {
public:
    /// asynchronously load a texture from an URL
    static Id Load(const TextureDesc& desc);
};

} // namespace Oryol
