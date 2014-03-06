#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::TextureWrapMode
    @brief texture coordinate wrapping modes
*/
#include "Core/Types.h"

namespace Oryol {
namespace Render {

class TextureWrapMode {
public:
    /// wrap modes
    enum Code {
        ClampToEdge = 0,            ///> clamp uv coords to [0,1]
        Repeat,                     ///> repeat (wrap-around) uv coords
        MirroredRepeat,             ///> mirror-repeat uv coords
        
        NumTextureWrapModes,        ///> number of texture wrap modes
        InvalidTextureWrapMode,     ///> the invalid texture wrap mode value
    };
    
    /// convert to string
    static const char* ToString(Code c);
    /// convert from string
    static Code FromString(const char* str);
};
    
} // namespace Render
} // namespace Oryol
 