#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::TextureFilterMode
    @brief texture sampling filter mode
*/
#include "Core/Types.h"

namespace Oryol {
namespace Render {
    
class TextureFilterMode {
public:
    /// filtering modes
    enum Code {
        Nearest,                    ///< nearest (point) filtering, no mipmapping
        Linear,                     ///< linear filtering, no mipmapping
        NearestMipmapNearest,       ///< nearest texel, nearest mipmap
        NearestMipmapLinear,        ///< nearest texel, linear mipmap filtering
        LinearMipmapNearest,        ///< linear texel filtering, nearest mipmap filtering
        LinearMipmapLinear,         ///< linear texel filtering, nearest mipmap filtering
        
        NumTextureFilterModes,      ///< number of texture filtering modes
        InvalidTextureFilterMode    ///< the invalid texture filtering mode value
    };
    
    /// convert to string
    static const char* ToString(Code c);
    /// convert from string
    static Code FromString(const char* str);
};

} // namespace Render
} // namespace Oryol