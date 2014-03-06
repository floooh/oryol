//------------------------------------------------------------------------------
//  PixelFormat.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "PixelFormat.h"
#include "Core/Macros.h"
#include <cstring>

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
const char*
PixelFormat::ToString(Code c) {
    switch (c) {
        __ORYOL_TOSTRING(R8G8B8A8);
        __ORYOL_TOSTRING(R8G8B8);
        __ORYOL_TOSTRING(R5G6B5);
        __ORYOL_TOSTRING(R5G5B5A1);
        __ORYOL_TOSTRING(R4G4B4A4);
        __ORYOL_TOSTRING(L8);
        __ORYOL_TOSTRING(DXT1);
        __ORYOL_TOSTRING(DXT3);
        __ORYOL_TOSTRING(DXT5);
        __ORYOL_TOSTRING(D16);
        __ORYOL_TOSTRING(D32);
        __ORYOL_TOSTRING(D24S8);
        __ORYOL_TOSTRING(R32F);
        __ORYOL_TOSTRING(PVRTC2);
        __ORYOL_TOSTRING(PVRTC4);
        default: return "InvalidPixelFormat";
    }
}

//------------------------------------------------------------------------------
PixelFormat::Code
PixelFormat::FromString(const char* str) {
    o_assert(str);
    __ORYOL_FROMSTRING(R8G8B8A8);
    __ORYOL_FROMSTRING(R8G8B8);
    __ORYOL_FROMSTRING(R5G6B5);
    __ORYOL_FROMSTRING(R5G5B5A1);
    __ORYOL_FROMSTRING(R4G4B4A4);
    __ORYOL_FROMSTRING(L8);
    __ORYOL_FROMSTRING(DXT1);
    __ORYOL_FROMSTRING(DXT3);
    __ORYOL_FROMSTRING(DXT5);
    __ORYOL_FROMSTRING(D16);
    __ORYOL_FROMSTRING(D32);
    __ORYOL_FROMSTRING(D24S8);
    __ORYOL_FROMSTRING(R32F);
    __ORYOL_FROMSTRING(PVRTC2);
    __ORYOL_FROMSTRING(PVRTC4);
    return InvalidPixelFormat;
}

} // namespace Render
} // namespace Oryol