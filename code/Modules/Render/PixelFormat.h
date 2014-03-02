#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::PixelFormat
    @brief enum of pixel formats
*/
#include "Core/Types.h"

namespace Oryol {
namespace Render {
    
class PixelFormat {
public:
    /// pixel format enum
    enum Code {
        // generic pixel formats
        X8R8G8B8 = 0,   ///> 32-bit wide, 3 usable channels @ 8-bit
        A8R8G8B8,       ///> 32-bit wide, 4 channels @ 8-bit
        R8G8B8,         ///> 24-bit wide, 3 channels @ 8-bit
        R5G6B5,         ///> 16-bit wide, 3 channels @ 5/6/5 bits
        A1R5G5B5,       ///> 16-bit wide, 4 channels @ 1-bit alpha, 5-bit rgb
        A4R4G4B4,       ///> 16-bit wide, 4 channels @ 4-bit
        L8,             ///> 8-bit wide, single channel
        
        // S3TC compressed pixel formats
        DXT1,           ///> DXT1 compressed format
        DXT3,           ///> DXT3 compressed format
        DXT5,           ///> DXT5 compressed format
        
        // depth pixel formats
        D16,            ///> 16-bit depth
        D24,            ///> 24-bit depth
        D32,            ///> 32-bit depth
        D24S8,          ///> 24-bit depth, 8-bit stencil
        
        // floating point pixel formats
        R32F,           ///> 32-bit wide, 1 channel @ 32-bit float
        
        // PVR formats
        PVRTC2,         ///> PVRTC2 compressed format
        PVRTC4,         ///> PVRTC4 compressed format
        
        NumPixelFormats,
        InvalidPixelFormat,
    };
    
    /// convert to string
    static const char* ToString(Code c);
    /// convert from string
    static Code FromString(const char* str);
};
    
} // namespace Render
} // namespace Oryol
