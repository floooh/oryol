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
        Nearest = 0,
        Linear,
        NearestMipmapNearest,
        NearestMipmapLinear,
        LinearMipmapNearest,
        LinearMipmapLinear,
        
        NumTextureFilterModes,
        InvalidTextureFilterMode
    };
    
    /// convert to string
    static const char* ToString(Code c);
    /// convert from string
    static Code FromString(const char* str);
};

} // namespace Render
} // namespace Oryol