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
        ClampToEdge = 0,
        Repeat,
        MirroredRepeat,
        
        NumTextureWrapModes,
        InvalidTextureWrapMode,
    };
    
    /// convert to string
    static const char* ToString(Code c);
    /// convert from string
    static Code FromString(const char* str);
};
    
} // namespace Render
} // namespace Oryol
 