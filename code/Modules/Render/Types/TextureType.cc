//------------------------------------------------------------------------------
//  TextureType.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "TextureType.h"
#include "Core/Macros.h"
#include <cstring>

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
const char*
TextureType::ToString(Code c) {
    switch (c) {
        __ORYOL_TOSTRING(Texture2D);
        __ORYOL_TOSTRING(Texture3D);
        __ORYOL_TOSTRING(TextureCube);
        default: return "InvalidTextureType";
    }
}

//------------------------------------------------------------------------------
TextureType::Code
TextureType::FromString(const char* str) {
    o_assert(str);
    __ORYOL_FROMSTRING(Texture2D);
    __ORYOL_FROMSTRING(Texture3D);
    __ORYOL_FROMSTRING(TextureCube);
    return InvalidTextureType;
}

} // namespace Render
} // namespace Oryol