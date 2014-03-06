#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::TextureType
    @brief texture type (1D, 2D, 3D, Cube)
    
    The type of data in a texture object (1D, 2D, 3D, Cube).
*/
#include "Core/Types.h"

namespace Oryol {
namespace Render {
class TextureType {
public:
    /// texture type enum
    enum Code {
        Texture2D,
        Texture3D,
        TextureCube,
        
        NumTextureTypes,
        InvalidTextureType,
    };
    
    /// convert TextureType to string
    static const char* ToString(Code c);
    /// convert string to TextureType
    static Code FromString(const char* str);
};
} // namespace Render
} // namespace Oryol
 