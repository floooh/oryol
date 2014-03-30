//------------------------------------------------------------------------------
//  PixelFormat.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "PixelFormat.h"
#include "Core/Macros.h"
#include "Core/Assert.h"
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

//------------------------------------------------------------------------------
bool
PixelFormat::IsValidRenderTargetColorFormat(Code c) {
    switch (c) {
        case R8G8B8A8:
        case R8G8B8:
        case R5G6B5:
        case R5G5B5A1:
        case R4G4B4A4:
            return true;
        default:
            return false;
    }
}

//------------------------------------------------------------------------------
bool
PixelFormat::IsValidRenderTargetDepthFormat(Code c) {
    switch (c) {
        case D16:
        case D32:
        case D24S8:
            return true;
        default:
            return false;
    }
}

//------------------------------------------------------------------------------
bool
PixelFormat::IsValidTextureColorFormat(Code c) {
    switch (c) {
        case D16:
        case D32:
        case D24S8:
            return false;
        default:
            return true;
    }
}

//------------------------------------------------------------------------------
bool
PixelFormat::IsValidTextureDepthFormat(Code c) {
    switch (c) {
        case D16:
        case D32:
        case D24S8:
            return true;
        default:
            return false;
    }
}

//------------------------------------------------------------------------------
bool
PixelFormat::IsCompressedFormat(Code c) {
    switch (c) {
        case DXT1:
        case DXT3:
        case DXT5:
        case PVRTC2:
        case PVRTC4:
            return true;
        default:
            return false;
    }
}

//------------------------------------------------------------------------------
int8
PixelFormat::NumBits(Code pixelFormat, Channel chn) {
    switch (pixelFormat) {
        case R8G8B8A8:
            if ((Red == chn) || (Green == chn) || (Blue == chn) || (Alpha == chn)) {
                return 8;
            }
            break;
        case R8G8B8:
            if ((Red == chn) || (Green == chn) || (Blue == chn)) {
                return 8;
            }
            break;
        case R5G6B5:
            if ((Red == chn) || (Blue == chn)) {
                return 5;
            }
            else if (Green == chn) {
                return 6;
            }
            break;
        case R5G5B5A1:
            if ((Red == chn) || (Green == chn) || (Blue == chn)) {
                return 5;
            }
            else if (Alpha == chn) {
                return 1;
            }
            break;
        case R4G4B4A4:
            if ((Red == chn) || (Green == chn) || (Blue == chn) || (Alpha == chn)) {
                return 4;
            }
            break;
        case L8:
            if (Red == chn) {
                return 8;
            }
            break;
        case D16:
            if (Depth == chn) {
                return 16;
            }
            break;
        case D32:
            if (Depth == chn) {
                return 32;
            }
            break;
        case D24S8:
            if (Depth == chn) {
                return 24;
            }
            else if (Stencil == chn) {
                return 8;
            }
            break;
        case R32F:
            if (Red == chn) {
                return 32;
            }
            break;
        default:
            break;
    }
    // fallthrough: unsupported combination
    return 0;
}

} // namespace Render
} // namespace Oryol