//------------------------------------------------------------------------------
//  TextureWrapMode.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Macros.h"
#include "TextureWrapMode.h"
#include <cstring>

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
const char*
TextureWrapMode::ToString(Code c) {
    switch (c) {
        __ORYOL_TOSTRING(ClampToEdge);
        __ORYOL_TOSTRING(Repeat);
        __ORYOL_TOSTRING(MirroredRepeat);
        default: return "InvalidTextureWrapMode";
    }
}

//------------------------------------------------------------------------------
TextureWrapMode::Code
TextureWrapMode::FromString(const char* str) {
    o_assert(str);
    __ORYOL_FROMSTRING(ClampToEdge);
    __ORYOL_FROMSTRING(Repeat);
    __ORYOL_FROMSTRING(MirroredRepeat);
    return InvalidTextureWrapMode;
}

} // namespace Render
} // namespace Oryol