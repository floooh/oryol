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
        R8G8B8A8,       ///< 32-bit wide, 4 channels @ 8-bit
        R8G8B8,         ///< 24-bit wide, 3 channels @ 8-bit
        R5G6B5,         ///< 16-bit wide, 3 channels @ 5/6/5 bits
        R5G5B5A1,       ///< 16-bit wide, 4 channels @ 1-bit alpha, 5-bit rgb
        R4G4B4A4,       ///< 16-bit wide, 4 channels @ 4-bit
        L8,             ///< 8-bit wide, single channel
        DXT1,           ///< DXT1 compressed format
        DXT3,           ///< DXT3 compressed format
        DXT5,           ///< DXT5 compressed format
        D16,            ///< 16-bit depth
        D32,            ///< 32-bit depth
        D24S8,          ///< 24-bit depth, 8-bit stencil
        R32F,           ///< 32-bit wide, 1 channel @ 32-bit float
        PVRTC2,         ///< PVRTC2 compressed format
        PVRTC4,         ///< PVRTC4 compressed format
        
        NumPixelFormats,            ///< number of pixel formats
        InvalidPixelFormat,         ///< invalid pixel format value
        None = InvalidPixelFormat,  ///< special "none" type
    };

    /// a pixel format channel
    enum Channel {
        Red,            ///< red bits
        Green,          ///< green bits
        Blue,           ///< blue bits
        Alpha,          ///< alpha bits
        Depth,          ///< depth bits
        Stencil,        ///< stencil bits

        NumChannels,    ///< number of channels
        InvalidChannel, ///< invalid channel value
    };
    
    /// convert to string
    static const char* ToString(Code c);
    /// convert from string
    static Code FromString(const char* str);
    /// return true for valid render target color formats
    static bool IsValidRenderTargetColorFormat(Code c);
    /// return true for valid render target depth formats
    static bool IsValidRenderTargetDepthFormat(Code c);
    /// return true for valid color texture formats
    static bool IsValidTextureColorFormat(Code c);
    /// return true for valid depth texture formats
    static bool IsValidTextureDepthFormat(Code c);
    /// test if the pixel format is a pure depth format (not a depth/stencil format)
    static bool IsDepthFormat(Code c);
    /// test if the pixel format is a depth/stencil format
    static bool IsDepthStencilFormat(Code c);
    /// return true if the pixel format is a compressed format
    static bool IsCompressedFormat(Code c);
    /// get number of bits in a pixel format channel (only for non-compressed formats!)
    static int8 NumBits(Code pixelFormat, Channel channel);
};
    
} // namespace Render
} // namespace Oryol
