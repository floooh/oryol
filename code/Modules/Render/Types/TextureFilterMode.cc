//------------------------------------------------------------------------------
//  TextureFilterMode.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "TextureFilterMode.h"
#include "Core/Assert.h"
#include "Core/Macros.h"
#include <cstring>

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
const char*
TextureFilterMode::ToString(Code c) {
    switch (c) {
        __ORYOL_TOSTRING(Nearest);
        __ORYOL_TOSTRING(Linear);
        __ORYOL_TOSTRING(NearestMipmapNearest);
        __ORYOL_TOSTRING(NearestMipmapLinear);
        __ORYOL_TOSTRING(LinearMipmapNearest);
        __ORYOL_TOSTRING(LinearMipmapLinear);
        default: return "InvalidTextureFilterMode";
    }
}

//------------------------------------------------------------------------------
TextureFilterMode::Code
TextureFilterMode::FromString(const char* str) {
    o_assert(str);
    __ORYOL_FROMSTRING(Nearest);
    __ORYOL_FROMSTRING(Linear);
    __ORYOL_FROMSTRING(NearestMipmapNearest);
    __ORYOL_FROMSTRING(NearestMipmapLinear);
    __ORYOL_FROMSTRING(LinearMipmapNearest);
    __ORYOL_FROMSTRING(LinearMipmapLinear);
    return TextureFilterMode::InvalidTextureFilterMode;
}

} // namespace Render
} // namespace Oryol