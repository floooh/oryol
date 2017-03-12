#pragma once
//------------------------------------------------------------------------------
/**
    @file Gfx/Core/Types.h
    @brief public Gfx types 
*/
#include "Core/Types.h"
#include "Core/Assertion.h"
#include "Core/String/String.h"
#include "Core/String/StringAtom.h"
#include "Resource/Id.h"
#include "Resource/Locator.h"
#include "Core/Containers/StaticArray.h"
#include "Gfx/Core/GfxConfig.h"
#include "glm/vec4.hpp"
#include <initializer_list>

namespace Oryol {

//------------------------------------------------------------------------------
/**
    @class Oryol::IndexType
    @ingroup Gfx
    @brief selects 16- or 32-bit indices
*/
class IndexType {
public:
    enum Code {
        None = 0,
        Index16,
        Index32,

        NumIndexTypes,
        InvalidIndexType = 0xFFFFFFFF
    };

    /// get byte size of index type
    static int ByteSize(IndexType::Code c) {
        switch (c) {
            case None:      return 0;
            case Index16:   return 2;
            case Index32:   return 4;
            default:
                o_error("IndexType::ByteSize() called with invalid type!\n");
                return 0;
        }
    }
};

//------------------------------------------------------------------------------
/**
    @class Oryol::PixelChannel
    @ingroup Gfx
    @brief RGBA/Depth/Stencil channel bits and combinations
 */
class PixelChannel {
public:
    typedef uint64_t Mask;
    enum Bits {
        None    = 0,

        Stencil = (1<<5),
        Depth   = (1<<4),
        Red     = (1<<3),
        Green   = (1<<2),
        Blue    = (1<<1),
        Alpha   = (1<<0),

        DepthStencil = Depth|Stencil,
        DS = DepthStencil,

        D = Depth,
        S = Stencil,
        R = Red,
        G = Green,
        B = Blue,
        A = Alpha,

        RG  = R|G,
        GB  = G|B,
        RB  = R|B,
        RGB = R|G|B,

        RA = R|A,
        GA = G|A,
        BA = B|A,

        RGA = R|G|A,
        GBA = G|B|A,
        RBA = R|B|A,

        RGBA = R|G|B|A,
        All = R|G|B|A|D|S,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::PixelFormat
    @ingroup Gfx
    @brief enum of pixel formats
*/
class PixelFormat {
public:
    #ifdef _MSC_VER // for correct bitfield packing, enum must be typed on MSVC
    enum Code : uint64_t {
    #else
    enum Code {
    #endif
        RGBA8,          ///< 32-bit wide, 4 channels @ 8-bit
        RGB8,           ///< 24-bit wide, 3 channels @ 8-bit
        RGBA4,          ///< 16-bit wide, 4 channels @ 4-bit
        R5G6B5,         ///< 16-bit wide, 3 channels @ 5/6/5 bits
        R5G5B5A1,       ///< 16-bit wide, 4 channels @ 1-bit alpha, 5-bit rgb
        R10G10B10A2,    ///< 32-bit wide, 4 channels @ 10/10/10/2 bits
        RGBA32F,        ///< 128-bit wide, 4 channel @ 32-bit float
        RGBA16F,        ///< 64-bit wide, 4 channel @ 16-bit float
        R32F,           ///< 32-bit wide, 1 channel @ 32-bit float
        R16F,           ///< 16-bit wide, 1 channel @ 16-bit float

        L8,             ///< 8-bit wide, single channel
        DXT1,           ///< DXT1 compressed format
        DXT3,           ///< DXT3 compressed format
        DXT5,           ///< DXT5 compressed format
        DEPTH,          ///< depth-only format (at least 16 bit)
        DEPTHSTENCIL,   ///< depth-stencil format (usually D24S8)
        PVRTC2_RGB,     ///< PVRTC2 compressed format (RGB)
        PVRTC4_RGB,     ///< PVRTC4 compressed format (RGB)
        PVRTC2_RGBA,    ///< PVRTC2 compressed format (RGBA)
        PVRTC4_RGBA,    ///< PVRTC4 compressed format (RGBA)
        ETC2_RGB8,      ///< ETC2 compressed format (RGB8)
        ETC2_SRGB8,     ///< ETC2 compressed format (SRGB8)

        NumPixelFormats,            ///< number of pixel formats
        InvalidPixelFormat,         ///< invalid pixel format value
        None = InvalidPixelFormat,  ///< special "none" type
    };

    /// return true for valid render target color formats
    /// FIXME: hmm according to GLES3 spec, the float formats
    /// are not renderable... does this need a separate extension?
    static bool IsValidRenderTargetColorFormat(Code c) {
        switch (c) {
            case RGBA8:
            case R10G10B10A2:
            case RGBA32F:
            case RGBA16F:
                return true;
            default:
                return false;
        }
    }
    /// return true for valid render target depth formats
    static bool IsValidRenderTargetDepthFormat(Code c) {
        switch (c) {
            case DEPTH:
            case DEPTHSTENCIL:
                return true;
            default:
                return false;
        }
    }
    /// return true for valid color texture formats
    static bool IsValidTextureColorFormat(Code c) {
        switch (c) {
            case DEPTH:
            case DEPTHSTENCIL:
                return false;
            default:
                return true;
        }
    }
    /// return true for valid depth texture formats
    static bool IsValidTextureDepthFormat(Code c) {
        switch (c) {
            case DEPTH:
            case DEPTHSTENCIL:
                return true;
            default:
                return false;
        }
    }
    /// test if the pixel format is a pure depth format (not a depth/stencil format)
    static bool IsDepthFormat(Code c) {
        return DEPTH == c;
    }
    /// test if the pixel format is a depth/stencil format
    static bool IsDepthStencilFormat(Code c) {
        return DEPTHSTENCIL == c;
    }
    /// return true if the pixel format is a compressed format
    static bool IsCompressedFormat(Code c) {
        switch (c) {
            case DXT1:
            case DXT3:
            case DXT5:
            case PVRTC2_RGB:
            case PVRTC4_RGB:
            case PVRTC2_RGBA:
            case PVRTC4_RGBA:
            case ETC2_RGB8:
            case ETC2_SRGB8:
                return true;
            default:
                return false;
        }
    }
    /// return true if this is a PVRTC-compressed format
    static bool IsPVRTC(Code c) {
        switch (c) {
            case PVRTC2_RGB:
            case PVRTC4_RGB:
            case PVRTC2_RGBA:
            case PVRTC4_RGBA:
                return true;
            default:
                return false;
        }
    }
    /// return true if this is a DXT-compressed format
    static bool IsDXT(Code c) {
        switch (c) {
            case DXT1:
            case DXT3:
            case DXT5:
                return true;
            default:
                return false;
        }
    }
    /// return true if this is an ETC-compressed format
    static bool IsETC2(Code c) {
        switch (c) {
            case ETC2_RGB8:
            case ETC2_SRGB8:
                return true;
            default:
                return false;
        }
    }
    /// get byte size of pixel format
    static int ByteSize(Code c) {
        switch (c) {
            case RGBA32F:
                return 16;
            case RGBA16F:
                return 8;
            case RGBA8:
            case R10G10B10A2:
            case R32F:
                return 4;
            case RGB8:
                return 3;
            case R5G6B5:
            case R5G5B5A1:
            case RGBA4:
            case R16F:
                return 2;
            case L8:
                return 1;
            case DEPTH:
                // NOTE: this is not true on all platform!
                return 2;
            case DEPTHSTENCIL:
                return 4;
            default:
                o_error("PixelFormat::ByteSize(): cannot get byte size for compressed format!\n");
                return 0;
        }
    }
    /// get number of bits in a pixel format channel (only for non-compressed formats and non-depth formats!)
    static int8_t NumBits(Code pixelFormat, PixelChannel::Bits channel) {
        switch (pixelFormat) {
            case RGBA32F:
                if ((PixelChannel::Red == channel) || (PixelChannel::Green == channel) || (PixelChannel::Blue == channel) || (PixelChannel::Alpha == channel)) {
                    return 32;
                }
                break;
            case RGBA16F:
                if ((PixelChannel::Red == channel) || (PixelChannel::Green == channel) || (PixelChannel::Blue == channel) || (PixelChannel::Alpha == channel)) {
                    return 16;
                }
                break;
            case R32F:
                if (PixelChannel::Red == channel) {
                    return 32;
                }
                break;
            case R16F:
                if (PixelChannel::Red == channel) {
                    return 16;
                }
                break;
            case R10G10B10A2:
                if ((PixelChannel::Red == channel) || (PixelChannel::Green == channel) || (PixelChannel::Blue == channel)) {
                    return 10;
                }
                else if (PixelChannel::Alpha == channel) {
                    return 2;
                }
                break;
            case RGBA8:
                if ((PixelChannel::Red == channel) || (PixelChannel::Green == channel) || (PixelChannel::Blue == channel) || (PixelChannel::Alpha == channel)) {
                    return 8;
                }
                break;
            case RGB8:
                if ((PixelChannel::Red == channel) || (PixelChannel::Green == channel) || (PixelChannel::Blue == channel)) {
                    return 8;
                }
                break;
            case R5G6B5:
                if ((PixelChannel::Red == channel) || (PixelChannel::Blue == channel)) {
                    return 5;
                }
                else if (PixelChannel::Green == channel) {
                    return 6;
                }
                break;
            case R5G5B5A1:
                if ((PixelChannel::Red == channel) || (PixelChannel::Green == channel) || (PixelChannel::Blue == channel)) {
                    return 5;
                }
                else if (PixelChannel::Alpha == channel) {
                    return 1;
                }
                break;
            case RGBA4:
                if ((PixelChannel::Red == channel) || (PixelChannel::Green == channel) || (PixelChannel::Blue == channel) || (PixelChannel::Alpha == channel)) {
                    return 4;
                }
                break;
            case L8:
                if (PixelChannel::Red == channel) {
                    return 8;
                }
                break;
            case DEPTH:
                // NOTE: this is not true on all platforms!
                if (PixelChannel::Depth == channel) {
                    return 16;
                }
                break;
            case DEPTHSTENCIL:
                if (PixelChannel::Depth == channel) {
                    return 24;
                }
                else if (PixelChannel::Stencil == channel) {
                    return 8;
                }
                break;
            default:
                break;
        }
        // fallthrough: unsupported combination
        return 0;
    }
    /// compute row-pitch (distance in bytes from one row of data to next)
    static int RowPitch(PixelFormat::Code fmt, int width) {
        int pitch;
        switch (fmt) {
            case PixelFormat::DXT1:
            case PixelFormat::ETC2_RGB8:
            case PixelFormat::ETC2_SRGB8:
                pitch = ((width + 3) / 4) * 8;
                pitch = pitch < 8 ? 8 : pitch;
                break;
            case PixelFormat::DXT3:
            case PixelFormat::DXT5:
                pitch = ((width + 3) / 4) * 16;
                pitch = pitch < 16 ? 16 : pitch;
                break;
            case PixelFormat::PVRTC4_RGB:
            case PixelFormat::PVRTC4_RGBA:
                {
                    const int blockSize = 4*4;
                    const int bpp = 4;
                    int widthBlocks = width / 4;
                    widthBlocks = widthBlocks < 2 ? 2 : widthBlocks;
                    pitch = widthBlocks * ((blockSize * bpp) / 8);
                }
                break;
            case PixelFormat::PVRTC2_RGB:
            case PixelFormat::PVRTC2_RGBA:
                {
                    const int blockSize = 8 * 4;
                    const int bpp = 2;
                    int widthBlocks = width / 4;
                    widthBlocks = widthBlocks < 2 ? 2 : widthBlocks;
                    pitch = widthBlocks * ((blockSize * bpp) / 8);
                }
                break;
            default:
                pitch = width * PixelFormat::ByteSize(fmt);
                break;
        }
        return pitch;
    }
    /// compute image-pitch (distance in bytes from one image to next)
    static int ImagePitch(PixelFormat::Code fmt, int width, int height) {
        int numRows = 0;
        switch (fmt) {
            case PixelFormat::DXT1:
            case PixelFormat::ETC2_RGB8:
            case PixelFormat::ETC2_SRGB8:
            case PixelFormat::DXT3:
            case PixelFormat::DXT5:
            case PixelFormat::PVRTC4_RGB:
            case PixelFormat::PVRTC4_RGBA:
            case PixelFormat::PVRTC2_RGB:
            case PixelFormat::PVRTC2_RGBA:
                numRows = ((height + 3) / 4);
                break;
            default:
                numRows = height;
                break;
        }
        if (numRows < 1) {
            numRows = 1;
        }
        const int pitch = numRows * RowPitch(fmt, width);
        return pitch;
    }
};

//------------------------------------------------------------------------------
/**
    @class Oryol::PrimitiveType
    @ingroup Gfx
    @brief primitive type enum (triangle strips, lists, etc...)
*/
class PrimitiveType {
public:
    /// primitive type enum (don't change order, append to end!)
    enum Code {
        Points = 0,
        Lines,
        LineStrip,
        Triangles,
        TriangleStrip,

        NumPrimitiveTypes,
        InvalidPrimitiveType = 0xFFFFFFFF,
    };

    /// convert primitive type to string
    static const char* ToString(PrimitiveType::Code c) {
        switch (c) {
            case Points:        return "Points";
            case Lines:         return "Lines";
            case LineStrip:     return "LineStrip";
            case Triangles:     return "Triangles";
            case TriangleStrip: return "TriangleStrip";
            default:
                o_error("PrimitiveType::ToString(): invalid value!\n");
                return 0;
        }
    }
};

//------------------------------------------------------------------------------
/**
    @class Oryol::GfxResourceType
    @ingroup Gfx
    @brief Gfx module resource types

    These types are used for the type in Id for Gfx module
    resources.
*/
class GfxResourceType {
public:
    /// type enum
    enum Code {
        Texture,            ///< a texture
        Mesh,               ///< a mesh
        Shader,             ///< a shader
        Pipeline,           ///< a pipeline state object
        RenderPass,         ///< a render-pass object

        NumResourceTypes,
        InvalidResourceType = 0xFFFF,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::ShaderStage
    @ingroup Gfx
    @brief the shader stages (vertex shader, fragment shader)
*/
class ShaderStage {
public:
    /// shader stages enum
    enum Code {
        VS = 0,
        FS,

        NumShaderStages,
        InvalidShaderStage = 0xFFFFFFFF,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::TextureFilterMode
    @ingroup Gfx
    @brief texture sampling filter mode
*/
class TextureFilterMode {
public:
    /// filtering modes
    #ifdef _MSC_VER // for correct bitfield packing, enum must be typed on MSVC
    enum Code : uint16_t {
    #else
    enum Code {
    #endif
        Nearest,
        Linear,
        NearestMipmapNearest,
        NearestMipmapLinear,
        LinearMipmapNearest,
        LinearMipmapLinear,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::TextureType
    @ingroup Gfx
    @brief texture type (2D, 3D, Cube)
*/
class TextureType {
public:
    /// texture type enum
    enum Code {
        Texture2D = 0,
        TextureCube,
        Texture3D,
        TextureArray,

        NumTextureTypes,
        InvalidTextureType = 0xFFFFFFFF,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::TextureWrapMode
    @ingroup Gfx
    @brief texture coordinate wrapping modes
*/
class TextureWrapMode {
public:
    /// wrap modes
    #ifdef _MSC_VER // for correct bitfield packing, enum must be typed on MSVC
    enum Code : uint16_t {
    #else
    enum Code {
    #endif
        ClampToEdge,
        Repeat,
        MirroredRepeat,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::Usage
    @ingroup Gfx
    @brief graphics resource usage types

    - Immutable:    requires initialization data
    - Dynamic:      update infrequently
    - Stream:       changed every frame
*/
class Usage {
public:
    /// usage enum
    enum Code {
        Immutable = 0,
        Dynamic,
        Stream,

        NumUsages,
        InvalidUsage = 0xFFFFFFFF,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::VertexAttr
    @ingroup Gfx
    @brief vertex attribute enum (position, texcoord, ...)

    The VertexAttr definitions don't have a hardwired meaning, they just
    exist to make the binding of vertex components (living in vertex buffers)
    to vertex attribute definition in vertex shaders easier to understand.
    The maximum number of vertex attributes should not exceed 16
    (this is the GL_MAX_VERTEX_ATTRIBS value).
*/
class VertexAttr {
public:
    /// vertex attribute enum
    enum Code : uint8_t {
        Position = 0,   ///< "position"
        Normal,         ///< "normal"
        TexCoord0,      ///< "texcoord0"
        TexCoord1,      ///< "texcoord1"
        TexCoord2,      ///< "texcoord2"
        TexCoord3,      ///< "texcoord3"
        Tangent,        ///< "tangent
        Binormal,       ///< "binormal"
        Weights,        ///< "weights" (skin weights)
        Indices,        ///< "indices" (skin indices)
        Color0,         ///< "color0"
        Color1,         ///< "color1"
        Instance0,      ///< "instance0"
        Instance1,      ///< "instance1"
        Instance2,      ///< "instance2"
        Instance3,      ///< "instance3"

        NumVertexAttrs,
        InvalidVertexAttr,
    };

    /// convert to string
    static const char* ToString(Code c) {
        switch (c) {
            case Position:  return "position";
            case Normal:    return "normal";
            case TexCoord0: return "texcoord0";
            case TexCoord1: return "texcoord1";
            case TexCoord2: return "texcoord2";
            case TexCoord3: return "texcoord3";
            case Tangent:   return "tangent";
            case Binormal:  return "binormal";
            case Weights:   return "weights";
            case Indices:   return "indices";
            case Color0:    return "color0";
            case Color1:    return "color1";
            case Instance0: return "instance0";
            case Instance1: return "instance1";
            case Instance2: return "instance2";
            case Instance3: return "instance3";
            default:
                o_error("VertexAttr::ToString(): invalid value!\n");
                return nullptr;
        }
    }
};

//------------------------------------------------------------------------------
/**
    @class Oryol::VertexFormat
    @ingroup Gfx
    @brief vertex component formats
    @see Vertex
    
    NOTE: The un-normalized integer formats are not compatible across
    GLES2 and D3D11! GLES2 needs to read those as float vec, but D3D11 
    can only read them as int vec!
*/
class VertexFormat {
public:
    /// format enum (don't change order, and append to end!)
    enum Code : uint8_t {
        Float,          ///< single component float, expanded to (x, 0, 0, 1)
        Float2,         ///< 2-component float, expanded to (x, y, 0, 1)
        Float3,         ///< 3-component float, expanded to (x, y, z, 1)
        Float4,         ///< 4-component float
        Byte4,          ///< 4-component float (-128.0f..+127.0f) mapped to byte (-128..+127)
        Byte4N,         ///< 4-component float (-1.0f..+1.0f) mapped to byte (-128..+127)
        UByte4,         ///< 4-component float (0.0f..255.0f) mapped to byte (0..255)
        UByte4N,        ///< 4-component float (0.0f..+1.0) mapped to byte (-128..+127)
        Short2,         ///< 2-component float (-32768.0f..+32767.0f) mapped to short (-32768..+32767)
        Short2N,        ///< 2-component float (-1.0f..+1.0f) mapped to short (-32768..+32767)
        Short4,         ///< 4-component float (-32768.0f..+32767.0f) mapped to short (-32768..+32767)
        Short4N,        ///< 4-component float (-1.0f..+1.0f) mapped to short (-32768..+32767)
        Int10_2N,       ///< 4-component packed, normalized 10-bit XYZ, 2-bit W (-1.0 .. +1.0)
        UInt10_2N,      ///< 4-component packed, normalized 10-bit XYZ, 2-bit W (0.0 .. 1.0)

        NumVertexFormats,       ///< number of vertex formats
        InvalidVertexFormat,    ///< the invalid vertex format value
    };

    /// get the byte size of a vertex format code
    static int ByteSize(Code c) {
        switch (c) {
            case Float:
                return 4;
            case Float2:
                return 8;
            case Float3:
                return 12;
            case Float4:
                return 16;
            case Byte4:
            case Byte4N:
            case UByte4:
            case UByte4N:
            case Short2:
            case Short2N:
            case Int10_2N:
            case UInt10_2N:
                return 4;
            case Short4:
            case Short4N:
                return 8;
            default:
                o_error("VertexFormat::ByteSize() called with invalid format!\n");
                return 0;
        }
    }

    // convert to string
    static const char* ToString(Code c) {
        switch (c) {
            case Float:     return "Float";
            case Float2:    return "Float2";
            case Float3:    return "Float3";
            case Float4:    return "Float4";
            case Byte4:     return "Byte4";
            case Byte4N:    return "Byte4N";
            case UByte4:    return "UByte4";
            case UByte4N:   return "Ubyte4N";
            case Short2:    return "Short2";
            case Short2N:   return "Short2N";
            case Short4:    return "Short4";
            case Short4N:   return "Short4N";
            case Int10_2N:  return "Int10_2N";
            case UInt10_2N: return "Int10_2N";
            default:
                o_error("VertexFormat::ToString(): invalid value!\n");
                return nullptr;
        }
    }
};

//------------------------------------------------------------------------------
/**
    @class Oryol::ShaderLang
    @ingroup Gfx
    @brief shader language syntax
*/
class ShaderLang {
public:
    enum Code {
        GLSL100 = 0,    ///< OpenGLES 2.0 / WebGL 1.0
        GLSL120,        ///< OpenGL 2.1
        GLSL330,        ///< OpenGL 3.3
        GLSLES3,        ///< OpenGLES3
        HLSL5,          ///< D3D11 HLSL
        Metal,          ///< Metal shader language

        NumShaderLangs,
        InvalidShaderLang
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::GfxFeature
    @ingroup Gfx
    @brief optional rendering features
*/
class GfxFeature {
public:
    enum Code {
        TextureCompressionDXT = 0,  ///< GPU supports DXT compressed textures
        TextureCompressionPVRTC,    ///< GPU supports PVRTC compressed textures
        TextureCompressionATC,      ///< GPU supports ATC compressed textures
        TextureCompressionETC2,     ///< GPU supports ETC2 compressed textures (OpenGLES3)
        TextureFloat,               ///< support for float textures
        TextureHalfFloat,           ///< support for half-float textures
        Instancing,                 ///< supports hardware-instanced rendering
        OriginBottomLeft,           ///< image space origin is bottom-left (GL-style)
        OriginTopLeft,              ///< image space origin is top-left (D3D-style)
        MSAARenderTargets,          ///< MSAA support in offscreen-render-targets
        PackedVertexFormat_10_2,    ///< support for VertexFormat Int10_2, Int10_2N, UInt10_2, UInt10_2N
        MultipleRenderTarget,       ///< support for MRT offscreen rendering
        Texture3D,                  ///< support for 3D textures
        TextureArray,               ///< support for array textures
        NativeTexture,              ///< can work with externally created texture objects

        NumFeatures,
        InvalidFeature
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::Face
    @ingroup Gfx
    @brief polygon face side (front, back, both)
*/
class Face {
public:
    #ifdef _MSC_VER // for correct bitfield packing, enum must be typed on MSVC
    enum Code : uint16_t {
    #else
    enum Code {
    #endif
        Front = 0,
        Back,
        Both,
    };
    static const int NumFaceCodes = 3;
    static const int NumSides = 2;
    static const int InvalidFace = 0xFF;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::CompareFunc
    @ingroup Gfx
    @brief comparison modes for depth and stencil state
*/
class CompareFunc {
public:
    #ifdef _MSC_VER // for correct bitfield packing, enum must be typed on MSVC
    enum Code : uint16_t {
    #else
    enum Code {
    #endif
        Never = 0,
        Less,
        Equal,
        LessEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always
    };
    static const int NumCompareFuncs = 8;
    static const int InvalidCompareFunc = 0xFF;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::StencilOperation
    @ingroup Gfx
    @brief stencil operations
*/
class StencilOp {
public:
    #ifdef _MSC_VER // for correct bitfield packing, enum must be typed on MSVC
    enum Code : uint16_t {
    #else
    enum Code {
    #endif
        Keep,
        Zero,
        Replace,
        IncrClamp,
        DecrClamp,
        Invert,
        IncrWrap,
        DecrWrap,
    };
    static const int NumStencilOperations = 8;
    static const int InvalidStencilOperation = 0xff;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::BlendFactor
    @ingroup Gfx
    @brief blending factors
*/
class BlendFactor {
public:
    #ifdef _MSC_VER // for correct bitfield packing, enum must be typed on MSVC
    enum Code : uint64_t {
    #else
    enum Code {
    #endif
        Zero = 0,
        One,
        SrcColor,
        OneMinusSrcColor,
        SrcAlpha,
        OneMinusSrcAlpha,
        DstColor,
        OneMinusDstColor,
        DstAlpha,
        OneMinusDstAlpha,
        SrcAlphaSaturated,
        BlendColor,
        OneMinusBlendColor,
        BlendAlpha,
        OneMinusBlendAlpha,
    };
    static const int NumBlendFactors = 15;
    static const int InvalidBlendFactor = 0xFF;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::BlendOperation
    @ingroup Gfx
    @brief blending operations
*/
class BlendOperation {
public:
    #ifdef _MSC_VER // for correct bitfield packing, enum must be typed on MSVC
    enum Code : uint64_t {
    #else
    enum Code {
    #endif
        Add = 0,
        Subtract,
        ReverseSubtract,
    };
    static const int NumBlendOperations = 3;
    static const int InvalidBlendOperation = 0xff;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::UniformType
    @ingroup Gfx
    @brief shader uniform types
*/
class UniformType {
public:
    enum Code : uint8_t {
        Float = 0,
        Vec2,
        Vec3,
        Vec4,
        Mat2,
        Mat3,
        Mat4,
        Int,
        Bool,

        NumUniformTypes,
        InvalidUniformType,
    };

    /// get the byte size of a uniform type, see GL std140 layout spec for details
    static int ByteSize(Code c, int numElements) {
        switch (c) {
            case Float:     return numElements * sizeof(float);
            case Vec2:      return numElements * 2 * sizeof(float);
            case Vec3:      return numElements * 4 * sizeof(float); // NOT A BUG
            case Vec4:      return numElements * 4 * sizeof(float);
            case Mat2:      return numElements * 2 * 2 * sizeof(float);
            case Mat3:      return numElements * 3 * 3 * sizeof(float); // FIXME!
            case Mat4:      return numElements * 4 * 4 * sizeof(float);
            case Int:       return numElements * sizeof(int);
            case Bool:      return numElements * sizeof(int);
            default:
                o_error("invalid scalar uniform type code!\n");
                return 0;
        }
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::VertexStepFunction
    @ingroup Gfx
    @brief classify vertices in a buffer as per-vertex or per-instance data
*/
class VertexStepFunction {
public:
    enum Code : uint8_t {
        PerVertex = 0,
        PerInstance = 1,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::PrimitiveGroup
    @ingroup Gfx
    @brief describes a group of primitives to be rendered
    
    A PrimitiveGroup object describes a range of primitive elements in
    a mesh, where elements are either vertices or indices.
*/
class PrimitiveGroup {
public:
    int BaseElement;
    int NumElements;

    /// default constructor
    PrimitiveGroup() :
        BaseElement(0),
        NumElements(0) {
        // empty
    }
    /// construct for indexed or non-indexed
    PrimitiveGroup(int baseElement, int numElements) :
        BaseElement(baseElement),
        NumElements(numElements) {
        // empty
    }
};

//------------------------------------------------------------------------------
/**
    @class Oryol::RenderPassLoadAction
    @ingroup Gfx
    @brief action to perform at start of a render pass
*/
class RenderPassLoadAction {
public:
    enum Code {
        DontCare,       // initial content of render target is undefined
        Load,           // previous content of render target is preserved
        Clear,          // render target pixels are initialized to a value
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::RenderPassStoreAction
    @ingroup Gfx
    @brief action to perform at end of render pass
*/
class RenderPassStoreAction {
public:
    enum Code {
        DontCare,           // content of render target is left undefined
        Store,              // content of render target is preserved
        Resolve,            // only perform an MSAA resolve into resolve target
        StoreAndResolve,    // preserve render target and perform MSAA resolve
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::BlendState
    @ingroup Gfx
    @brief describe alpha blending state
*/
class BlendState {
public:
    union {
        #pragma pack(push,1)
        struct {
            uint64_t BlendEnabled:1;
            BlendFactor::Code SrcFactorRGB:5;
            BlendFactor::Code DstFactorRGB:5;
            BlendOperation::Code OpRGB:3;
            BlendFactor::Code SrcFactorAlpha:5;
            BlendFactor::Code DstFactorAlpha:5;
            BlendOperation::Code OpAlpha:3;
            PixelChannel::Mask ColorWriteMask:4;
            PixelFormat::Code ColorFormat : 5;
            PixelFormat::Code ColorFormat3 : 5;
            PixelFormat::Code DepthFormat : 5;
            uint64_t MRTCount : 3;
        };
        #pragma pack(pop)
        /// hash code from merged state
        uint64_t Hash;
    };

    /// constructor
    BlendState() {
        static_assert(sizeof(BlendState) == 8, "sizeof(BlendState) is not 8, bitfield packing problem?");
        this->Hash = 0;
        this->BlendEnabled = false;
        this->SrcFactorRGB = BlendFactor::One;
        this->DstFactorRGB = BlendFactor::Zero;
        this->OpRGB = BlendOperation::Add;
        this->SrcFactorAlpha = BlendFactor::One;
        this->DstFactorAlpha = BlendFactor::Zero;
        this->OpAlpha = BlendOperation::Add;
        this->ColorWriteMask = PixelChannel::RGBA;
        this->ColorFormat = PixelFormat::RGBA8;
        this->DepthFormat = PixelFormat::DEPTHSTENCIL;
        this->MRTCount = 1;
    };
    
    /// equality
    bool operator==(const BlendState& rhs) const {
        return this->Hash == rhs.Hash;
    };
    /// inequality
    bool operator!=(const BlendState& rhs) const {
        return this->Hash != rhs.Hash;
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::StencilState
    @ingroup Gfx
    @brief holds stencil-buffer render state for one face side
*/
class StencilState {
public:

    union {
        #pragma pack(push, 1)
        struct {
            StencilOp::Code FailOp : 4;
            StencilOp::Code DepthFailOp : 4;
            StencilOp::Code PassOp : 4;
            CompareFunc::Code CmpFunc : 4;
        };
        #pragma pack(pop)
        uint16_t Hash;
    };

    /// constructor
    StencilState() {
        static_assert(sizeof(StencilState) == 2, "sizeof(StencilState) is not 2, bitfield packing problem?");
        this->Hash = 0;
        this->FailOp = StencilOp::Keep;
        this->DepthFailOp = StencilOp::Keep;
        this->PassOp = StencilOp::Keep;
        this->CmpFunc = CompareFunc::Always;
    }
    
    /// equality
    bool operator==(const StencilState& rhs) const {
        return this->Hash == rhs.Hash;
    };
    /// inequality
    bool operator!=(const StencilState& rhs) const {
        return this->Hash != rhs.Hash;
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::DepthStencilState
    @ingroup Gfx
    @brief holds the complete depth and stencil render state
*/
class DepthStencilState {
public:
    /// front-side stencil state
    StencilState StencilFront;
    /// back-side stencil state
    StencilState StencilBack;
    /// common depth-stencil state
    union {
        struct {
            /// depth compare-function
            CompareFunc::Code DepthCmpFunc:5;
            /// depth write enabled flag
            uint16_t DepthWriteEnabled:1;
            /// stencil-enabled flag
            uint16_t StencilEnabled:1;
            /// stencil read-mask
            uint16_t StencilReadMask : 8;
            /// stencil write-mask
            uint16_t StencilWriteMask : 8;
            /// stencil-ref value
            uint16_t StencilRef : 8;
        };
        uint32_t Hash;
    };

    /// constructor
    DepthStencilState() {
        static_assert(sizeof(DepthStencilState) == 8, "sizeof(DepthStencilState) is not 8, bitfield packing problem?");
        this->Hash = 0;
        this->DepthCmpFunc = CompareFunc::Always;
        this->DepthWriteEnabled = false;
        this->StencilEnabled = false;
        this->StencilReadMask = 0xFF;
        this->StencilWriteMask = 0xFF;
        this->StencilRef = 0;
    };

    /// equality
    bool operator==(const DepthStencilState& rhs) const {
        return (this->Hash == rhs.Hash) &&
               (this->StencilFront == rhs.StencilFront) &&
               (this->StencilBack == rhs.StencilBack);
    };
    /// inequality
    bool operator!=(const DepthStencilState& rhs) const {
        return (this->Hash != rhs.Hash) ||
               (this->StencilFront != rhs.StencilFront) ||
               (this->StencilBack != rhs.StencilBack);
    }
};

//------------------------------------------------------------------------------
/**
    @class Oryol::RasterizerState
    @ingroup Gfx
    @brief rasterizer state flags
*/
class RasterizerState {
public:
    union {
        #pragma pack(push,1)
        struct {
            uint16_t CullFaceEnabled : 1;
            uint16_t DepthOffsetEnabled : 1;
            uint16_t ScissorTestEnabled : 1;
            uint16_t DitherEnabled : 1;
            uint16_t AlphaToCoverageEnabled : 1;
            Face::Code CullFace : 3;
            uint16_t SampleCount : 4;
        };
        #pragma pack(pop)
        uint16_t Hash;
    };

    /// constructor
    RasterizerState() {
        static_assert(sizeof(RasterizerState) == 2, "sizeof(RasterizerState) is not 4, bitfield packing problem?");
        this->Hash = 0;
        this->CullFaceEnabled = false;
        this->DepthOffsetEnabled = false;
        this->ScissorTestEnabled = false;
        this->DitherEnabled = true;
        this->AlphaToCoverageEnabled = false;
        this->CullFace = Face::Back;
        this->SampleCount = 1;
    }
    /// equality
    bool operator==(const RasterizerState& rhs) const {
        return this->Hash == rhs.Hash;
    };
    /// inequality
    bool operator!=(const RasterizerState& rhs) const {
        return this->Hash != rhs.Hash;
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::SamplerState
    @ingroup Gfx
    @brief wrap texture sampler state
*/
class SamplerState {
public:
    union {
        #pragma pack(push, 1)
        struct {
            /// texture-wrap mode for u-axis
            TextureWrapMode::Code WrapU : 2;
            /// texture-wrap mode for v-axis
            TextureWrapMode::Code WrapV : 2;
            /// texture-wrap mode for w-axis
            TextureWrapMode::Code WrapW : 2;
            /// magnification filter
            TextureFilterMode::Code MagFilter : 3;
            /// minification filter
            TextureFilterMode::Code MinFilter : 3;
        };
        #pragma pack(pop)
        uint16_t Hash;
    };

    /// constructor
    SamplerState() {
        static_assert(sizeof(SamplerState) == 2, "sizeof(SamplerState) is not 2, bitfield packing problem?");
        this->Hash = 0;
        this->WrapU = TextureWrapMode::Repeat;
        this->WrapV = TextureWrapMode::Repeat;
        this->WrapW = TextureWrapMode::Repeat;
        this->MagFilter = TextureFilterMode::Nearest;
        this->MinFilter = TextureFilterMode::Nearest;
    };
    /// equality
    bool operator==(const SamplerState& rhs) const {
        return this->Hash == rhs.Hash;
    };
    /// inequality
    bool operator!=(const SamplerState& rhs) const {
        return this->Hash != rhs.Hash;
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::PassState
    @ingroup Gfx
    @brief describes override parameters for Gfx::BeginPass()
*/
class PassState {
public:
    /// default constructor
    PassState() : Depth(1.0f), Stencil(0) {
        for (auto& c : this->Color) {
            c = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        }
    };
    /// construct with clear color (all attachment to same color), optional depth/stencil
    PassState(const glm::vec4& color, float depth=1.0f, uint8_t stencil=0) {
        for (auto& c : this->Color) {
            c = color;
        }
        this->Depth = depth;
        this->Stencil = stencil;
    }
    /// construct with MRT clear colors
    PassState(std::initializer_list<glm::vec4> colors, float depth=1.0f, uint8_t stencil=0) {
        int i = 0;
        for (const auto& c : colors) {
            this->Color[i++] = c;
        }
        this->Depth = depth;
        this->Stencil = stencil;
    }

    /// override clear colors
    StaticArray<glm::vec4, GfxConfig::MaxNumColorAttachments> Color;
    /// override clear depth value
    float Depth;
    /// override clear-stencil value
    uint8_t Stencil;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::DrawState
    @brief state required to issue draw calls
    
    The DrawState struct contains state required to issue draw calls
    with the exception of shader uniforms:
    
    - 1 pipeline state object
    - 1..4 mesh objects
    - 0..N textures for the vertex shader stage
    - 0..N textures for the fragment shader stage
*/
struct DrawState {
    /// the pipeline state object
    Id Pipeline;
    /// input meshes
    StaticArray<Id, GfxConfig::MaxNumInputMeshes> Mesh;
    /// vertex shader stage textures
    StaticArray<Id, GfxConfig::MaxNumVertexTextures> VSTexture;
    /// fragment shader stage textures
    StaticArray<Id, GfxConfig::MaxNumFragmentTextures> FSTexture;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::GfxFrameInfo
    @brief per-frame stats of the Gfx module
*/
struct GfxFrameInfo {
    int NumPasses = 0;
    int NumApplyViewPort = 0;
    int NumApplyScissorRect = 0;
    int NumApplyDrawState = 0;
    int NumApplyUniformBlock = 0;
    int NumUpdateVertices = 0;
    int NumUpdateIndices = 0;
    int NumUpdateTextures = 0;
    int NumDraw = 0;
    int NumDrawInstanced = 0;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::VertexLayout
    @ingroup Gfx
    @brief describes the data layout of a vertex in a vertex buffer
*/
class VertexLayout {
public:
    /// a component in a vertex layout
    #pragma pack(push,1)
    class Component {
    public:
        /// default constructor
        Component() {};
        /// construct from vertex attr and format
        Component(VertexAttr::Code attr, VertexFormat::Code fmt) : Attr(attr), Format(fmt) { }
        /// return true if valid (attr and format set)
        bool IsValid() const {
            return (VertexAttr::InvalidVertexAttr != this->Attr);
        }
        /// clear the component (unset attr and format)
        void Clear() {
            this->Attr = VertexAttr::InvalidVertexAttr;
            this->Format = VertexFormat::InvalidVertexFormat;
        }
        /// get byte size of component
        int ByteSize() const {
            return VertexFormat::ByteSize(this->Format);
        }
        VertexAttr::Code Attr = VertexAttr::InvalidVertexAttr;
        VertexFormat::Code Format = VertexFormat::InvalidVertexFormat;
    };
    #pragma pack(pop)

    /// the vertex step function, used for instancing, default is 'PerVertex'
    VertexStepFunction::Code StepFunction = VertexStepFunction::PerVertex;
    /// the vertex step rate, used for instancing
    uint8_t StepRate = 1;

    /// constructor
    VertexLayout() {
        this->Clear();
    }
    /// clear the vertex layout, chainable
    VertexLayout& Clear() {
        this->StepFunction = VertexStepFunction::PerVertex;
        this->StepRate = 1;
        this->numComps = 0;
        this->byteSize = 0;
        this->attrCompIndices.Fill(InvalidIndex);
        this->byteOffsets.Fill(0);
        return *this;
    }
    /// return true if layout is empty
    bool Empty() const {
        return 0 == this->numComps;
    }
    /// add a component
    VertexLayout& Add(const Component& comp) {
        o_assert_dbg(this->numComps < GfxConfig::MaxNumVertexLayoutComponents);
        o_assert_dbg(InvalidIndex == this->attrCompIndices[comp.Attr]);
        this->comps[this->numComps] = comp;
        this->attrCompIndices[comp.Attr] = this->numComps;
        this->byteOffsets[this->numComps] = this->byteSize;
        this->byteSize += comp.ByteSize();
        o_assert_dbg(this->byteSize < 248);
        this->numComps++;
        return *this;
    }
    /// add component by name and format
    VertexLayout& Add(VertexAttr::Code attr, VertexFormat::Code format) {
        return this->Add(Component(attr, format));
    }
    /// enable layout for instancing, set StepFunction to PerInstance and StepRate to 1
    VertexLayout& EnableInstancing() {
        this->StepFunction = VertexStepFunction::PerInstance;
        this->StepRate = 1;
        return *this;
    }
    /// get number of components
    int NumComponents() const {
        return this->numComps;
    }
    /// get component at index
    const Component& ComponentAt(int index) const {
        return this->comps[index];
    }
    /// get component index by vertex attribute, return InvalidIndex if layout doesn't include attr
    int ComponentIndexByVertexAttr(VertexAttr::Code attr) const {
        return this->attrCompIndices[attr];
    }
    /// get byte size of vertex (aka stride)
    int ByteSize() const {
        return this->byteSize;
    }
    /// get byte offset of a component
    int ComponentByteOffset(int componentIndex) const {
        return this->byteOffsets[componentIndex];
    }
    /// test if the layout contains a specific vertex attribute
    bool Contains(VertexAttr::Code attr) const {
        return InvalidIndex != this->ComponentIndexByVertexAttr(attr);
    }
private:
    StaticArray<Component, GfxConfig::MaxNumVertexLayoutComponents> comps;
    StaticArray<uint8_t, GfxConfig::MaxNumVertexLayoutComponents> byteOffsets;
    StaticArray<int8_t, VertexAttr::NumVertexAttrs> attrCompIndices;  // maps vertex attributes to component indices
    int8_t numComps = 0;
    uint8_t byteSize = 0;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::TextureBlockLayout
    @ingroup Gfx
    @brief describes the name, type and bindSlot of a group of related textures
*/
class TextureBlockLayout {
public:
    /// a TextureBlockLayout component describes a single texture entry
    class Component {
    public:
        /// default constructor
        Component() {};
        /// construct from name, type and bindSlot
        Component(const StringAtom& name, TextureType::Code type, int bindSlot) :
            Name(name), Type(type), BindSlot(bindSlot) { }
        /// return true if the component is valid
        bool IsValid() const {
            return this->Name.IsValid();
        }
        StringAtom Name; 
        TextureType::Code Type = TextureType::InvalidTextureType;
        int BindSlot = InvalidIndex;
    };
    /// clear the texture layout
    void Clear() {
        this->comps.Fill(Component());
        this->numComps = 0;
    }
    /// return true if the layout is empty
    bool Empty() const {
        return 0 == this->numComps;
    }
    /// add a component to the layout
    TextureBlockLayout& Add(const Component& comp) {
        this->comps[this->numComps++] = comp;
        return *this;
    }
    /// add a component to the layout
    TextureBlockLayout& Add(const StringAtom& name, TextureType::Code type, int bindSlot) {
        this->comps[this->numComps++] = Component(name, type, bindSlot);
        return *this;
    }
    /// get number of components in the layout
    int NumComponents() const {
        return this->numComps;
    }
    /// find component index with matching bind slot, InvalidIndex if not match
    int ComponentIndexForBindSlot(int bindSlot) const {
        for (int i = 0; i < this->numComps; i++) {
            if (this->comps[i].BindSlot == bindSlot) {
                return i;
            }
        }
        return InvalidIndex;
    }
    /// get component at index
    const Component& ComponentAt(int index) const {
        return this->comps[index];
    }
private:
    int numComps = 0;
    StaticArray<Component, GfxConfig::MaxNumTextureBlockLayoutComponents> comps;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::UniformBlockLayout
    @ingroup Gfx
    @brief describes the layout of an uniform block

    A UniformBlockLayout describes the names and types of a group of
    related shader uniforms.
*/
class UniformBlockLayout {
public:
    /// a UniformBlockLayout component describes a single uniform in the uniform block
    class Component {
    public:
        /// default constructor
        Component() {}
        /// construct from name, type and number
        Component(const StringAtom& name, UniformType::Code type, int num=1):
            Name(name), Type(type), Num(num) {
            o_assert_dbg(this->Name.IsValid());
            o_assert_dbg(this->Type < UniformType::NumUniformTypes);
        }
        /// return true if the component is valid
        bool IsValid() const {
            return this->Name.IsValid();
        }
        /// compute the byte size of the component
        int ByteSize() const {
            return UniformType::ByteSize(this->Type, this->Num);
        }
        StringAtom Name; 
        UniformType::Code Type = UniformType::InvalidUniformType;
        int Num = 1;
    };
    /// clear the uniform layout
    void Clear() {
        this->comps.Fill(Component());
        this->byteOffsets.Fill(0);
        this->numComps = 0;
        this->byteSize = 0;
    }
    /// return true if the layout is empty
    bool Empty() const {
        return 0 == this->numComps;
    }
    /// a layout type hash, this is used for runtime type checking in Gfx::ApplyUniformBlock
    uint32_t TypeHash = 0;
    /// add a uniform component to the layout
    UniformBlockLayout& Add(const Component& comp) {
        this->comps[this->numComps] = comp;
        this->byteOffsets[this->numComps] = this->byteSize;
        this->byteSize += comp.ByteSize();
        this->numComps++;
        return *this;
    }
    /// add a scalar uniform component to the layout
    UniformBlockLayout& Add(const StringAtom& name, UniformType::Code type, int numElements=1) {
        return this->Add(Component(name, type, numElements));
    }
    /// get number of components in the layout
    int NumComponents() const {
        return this->numComps;
    }
    /// get component at index
    const Component& ComponentAt(int componentIndex) const {
        return this->comps[componentIndex];
    }
    /// get the overall byte size of the uniform layout
    int ByteSize() const {
        return this->byteSize;
    }
    /// get byte offset of a component
    int ComponentByteOffset(int componentIndex) const {
        return this->byteOffsets[componentIndex];
    }
private:
    int numComps = 0;
    int byteSize = 0;
    StaticArray<Component, GfxConfig::MaxNumUniformBlockLayoutComponents> comps;
    StaticArray<int, GfxConfig::MaxNumUniformBlockLayoutComponents> byteOffsets;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::TextureAttrs
    @ingroup Gfx
    @brief holds the public attributes of a Texture object
    
    @todo: describe TextureAttrs
*/
struct TextureAttrs {
    /// texture locator (usually the URL of the texture file)
    class Locator Locator;
    /// the texture type (2D, 3D, cube...)
    TextureType::Code Type = TextureType::InvalidTextureType;
    /// the RGBA pixel format of texture data
    PixelFormat::Code ColorFormat = PixelFormat::InvalidPixelFormat;
    /// optional depth format (only used for render target textures)
    PixelFormat::Code DepthFormat = PixelFormat::InvalidPixelFormat;
    /// optional sample count (only used for MSAA render target textures)
    int SampleCount = 1;
    /// texture usage hint
    Usage::Code TextureUsage = Usage::InvalidUsage;
    /// width of top-level mipmap in pixels
    int Width = 0;
    /// height of top-level mipmap in pixels
    int Height = 0;
    /// depth of top-level mipmap in pixels (only used for 3D textures)
    int Depth = 0;
    /// number of mipmaps (1 for 'no child mipmaps')
    int NumMipMaps = 1;
    /// true if this is a render target texture
    bool IsRenderTarget = false;
    /// true if this render target texture has an attached depth buffer
    bool HasDepthBuffer = false;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::DisplayAttrs
    @ingroup Gfx
    @brief actual display attributes
    
    Holds the properties of the current display, these can be 
    different from the display setup parameters.
*/
struct DisplayAttrs {
    /// window width (including window chrome)
    int WindowWidth = 0;
    /// window height (including window chrome)
    int WindowHeight = 0;
    /// x-position of window
    int WindowPosX = 0;
    /// y-position of window
    int WindowPosY = 0;
    /// width of framebuffer associated with window
    int FramebufferWidth = 0;
    /// height of framebuffer associated with window
    int FramebufferHeight = 0;
    /// framebuffer pixel format
    PixelFormat::Code ColorPixelFormat = PixelFormat::RGBA8;
    /// depth buffer pixel format (PixelFormat::None if no depth buffer)
    PixelFormat::Code DepthPixelFormat = PixelFormat::DEPTHSTENCIL;
    /// number of multisample-anti-aliasing samples
    int SampleCount = 1;
    /// indicates windowed or fullscreen mode
    bool Windowed = true;
    /// vsync swap interval (0 means: no vsync)
    int SwapInterval = 1;
    /// window title as UTF-8
    String WindowTitle;

    /// init a DisplayAttrs object from a TextureAttrs object
    static DisplayAttrs FromTextureAttrs(const TextureAttrs& texAttrs) {
        DisplayAttrs dispAttrs;
        dispAttrs.WindowWidth = texAttrs.Width;
        dispAttrs.WindowHeight = texAttrs.Height;
        dispAttrs.WindowPosX = 0;
        dispAttrs.WindowPosY = 0;
        dispAttrs.FramebufferWidth = texAttrs.Width;
        dispAttrs.FramebufferHeight = texAttrs.Height;
        dispAttrs.ColorPixelFormat = texAttrs.ColorFormat;
        dispAttrs.DepthPixelFormat = texAttrs.DepthFormat;
        dispAttrs.SampleCount = texAttrs.SampleCount;
        dispAttrs.Windowed = false;
        dispAttrs.SwapInterval = 1;
        return dispAttrs;
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::GfxEvent
    @brief an event from the Gfx module
    
    Subscribe to GfxEvents with the Gfx::Subscribe() method.
*/
class GfxEvent {
public:
    /// event types
    enum Type {
        DisplayModified,

        NumTypes,
        InvalidType
    };
    /// default constructor
    GfxEvent();
    /// constructor with arguments
    GfxEvent(Type type, const DisplayAttrs& attrs) : Type(type), DisplayAttrs(attrs) { }

    enum Type Type = InvalidType;
    struct DisplayAttrs DisplayAttrs;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::ImageDataAttrs
    @brief describe offsets and sizes of image surfaces
*/
class ImageDataAttrs {
public:
    /// constructor
    ImageDataAttrs() {
        for (auto& offsets : this->Offsets) {
            offsets.Fill(0);
        }
        for (auto& sizes : this->Sizes) {
            sizes.Fill(0);
        }
    };

    /// number of faces
    int NumFaces = 0;
    /// number of mipmaps
    int NumMipMaps = 0;
    /// pixel data mipmap image offsets
    StaticArray<StaticArray<int, GfxConfig::MaxNumTextureMipMaps>, GfxConfig::MaxNumTextureFaces> Offsets;
    /// pixel data mipmap image sizes
    StaticArray<StaticArray<int, GfxConfig::MaxNumTextureMipMaps>, GfxConfig::MaxNumTextureFaces> Sizes;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::IndexBufferAttrs
    @ingroup Gfx
    @brief attributes of an index buffer
*/
struct IndexBufferAttrs {
    /// number of indices in the index buffer
    int NumIndices{0};
    /// type of indices (16-bit or 32-bit)
    IndexType::Code Type{IndexType::InvalidIndexType};
    /// buffer usage hint
    Usage::Code BufferUsage{Usage::InvalidUsage};
    /// computes the byte size of index buffer data
    int ByteSize() const {
        return NumIndices * IndexType::ByteSize(Type);
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::VertexBufferAttrs
    @ingroup Gfx
    @brief attributes of one vertex buffer
*/
struct VertexBufferAttrs {
    /// number of vertices in the vertex buffer
    int NumVertices = 0;
    /// describes the vertex layout of a vertex in the buffer
    VertexLayout Layout;
    /// buffer usage hint
    Usage::Code BufferUsage = Usage::InvalidUsage;
    /// computes the byte size of the contained vertex buffer data
    int ByteSize() const {
        return NumVertices * Layout.ByteSize();
    }
};

//------------------------------------------------------------------------------
/**
    @class Oryol::GfxSetup
    @ingroup Gfx
    @brief Gfx module setup parameters
    
    The GfxSetup object holds the parameter used to setup the
    rendering system. Create a GfxSetup object, optionally tweak
    its values, and call the Gfx::Setup method with the
    GfxSetup object as argument.
 
    @see Gfx, DisplayAttrs
*/
class GfxSetup {
public:
    /// shortcut for windowed mode (with RGBA8, 24+8 stencil/depth, no MSAA)
    static GfxSetup Window(int width, int height, String windowTitle) {
        o_assert_dbg((width > 0) && (height > 0));
        GfxSetup setup;
        setup.Width    = width;
        setup.Height   = height;
        setup.Windowed = true;
        setup.Title    = windowTitle;
        return setup;
    }
    /// shortcut for fullscreen mode (with RGBA8, 24+8 stencil/depth, no MSAA)
    static GfxSetup Fullscreen(int width, int height, String windowTitle) {
        o_assert_dbg((width > 0) && (height > 0));
        GfxSetup setup;
        setup.Width    = width;
        setup.Height   = height;
        setup.Windowed = false;
        setup.Title    = windowTitle;
        return setup;
    }
    /// shortcut for windowed mode with 4xMSAA (with RGBA8, 24+8 stencil/depth)
    static GfxSetup WindowMSAA4(int width, int height, String windowTitle) {
        GfxSetup setup = Window(width, height, windowTitle);
        setup.SampleCount = 4;
        return setup;
    }
    /// shortcut for fullscreen mode with 4xMSAA (with RGBA8, 24+8 stencil/depth)
    static GfxSetup FullscreenMSAA4(int width, int height, String windowTitle) {
        GfxSetup setup = Fullscreen(width, height, windowTitle);
        setup.SampleCount = 4;
        return setup;
    }
    /// canvas width
    int Width = 640;
    /// canvas height
    int Height = 400;
    /// color pixel format
    PixelFormat::Code ColorFormat = PixelFormat::RGBA8;
    /// depth pixel format
    PixelFormat::Code DepthFormat = PixelFormat::DEPTHSTENCIL;
    /// MSAA samples (2, 4, 8... no MSAA: 1)
    int SampleCount = 1;
    /// windowed vs Fullscreen
    bool Windowed = true;
    /// swap interval (0 => no vsync, default is 1)
    int SwapInterval = 1;
    /// window title
    String Title = "Oryol";
    /// enable to render full-res on HighDPI displays (not supported on all platforms)
    bool HighDPI = false;
    /// color load action for the default-render-pass
    RenderPassLoadAction::Code DefaultColorLoadAction = RenderPassLoadAction::Clear;
    /// depth buffer load action for the default-render-pass
    RenderPassLoadAction::Code DefaultDepthStencilLoadAction = RenderPassLoadAction::Clear;
    /// default clear color
    glm::vec4 DefaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    /// default clear depth value
    float DefaultClearDepth = 1.0f;
    /// default clear stencil value
    uint8_t DefaultClearStencil = 0;
    /// if true, ignore own size and instead track size of an HTML element (emscripten only)
    bool HtmlTrackElementSize = false;
    /// name of the HTML element to track (default: #canvas)
    String HtmlElement = "#canvas";
    /// resource pool size by resource type
    StaticArray<int,GfxResourceType::NumResourceTypes> ResourcePoolSize;
    /// resource creation throttling (max resources created async per frame)
    StaticArray<int,GfxResourceType::NumResourceTypes> ResourceThrottling;
    /// initial resource label stack capacity
    int ResourceLabelStackCapacity = 256;
    /// initial resource registry capacity
    int ResourceRegistryCapacity = 256;
    /// size of the global uniform buffer (only relevant on some platforms)
    int GlobalUniformBufferSize = GfxConfig::DefaultGlobalUniformBufferSize;
    /// max number of drawcalls per frame (only relevant on some platforms)
    int MaxDrawCallsPerFrame = GfxConfig::DefaultMaxDrawCallsPerFrame;
    /// max number of ApplyDrawState per frame (only relevant on some platforms)
    int MaxApplyDrawStatesPerFrame = GfxConfig::DefaultMaxApplyDrawStatesPerFrame;
    /// get DisplayAttrs object initialized to setup values
    DisplayAttrs GetDisplayAttrs() const {
        DisplayAttrs attrs;
        attrs.WindowWidth       = this->Width;
        attrs.WindowHeight      = this->Height;
        attrs.WindowPosX        = 0;
        attrs.WindowPosY        = 0;
        attrs.FramebufferWidth  = this->Width;
        attrs.FramebufferHeight = this->Height;
        attrs.ColorPixelFormat  = this->ColorFormat;
        attrs.DepthPixelFormat  = this->DepthFormat;
        attrs.SampleCount       = this->SampleCount;
        attrs.Windowed          = this->Windowed;
        attrs.WindowTitle       = this->Title;
        attrs.SwapInterval      = this->SwapInterval;
        return attrs;
    }
    /// default constructor
    GfxSetup() {
        for (int i = 0; i < GfxResourceType::NumResourceTypes; i++) {
            ResourcePoolSize[i] = GfxConfig::DefaultResourcePoolSize;
            ResourceThrottling[i] = 0;    // unthrottled
        }
    }
};

//------------------------------------------------------------------------------
/**
    @class Oryol::MeshSetup
    @ingroup Gfx
    @brief setup attributes for meshes
*/
class MeshSetup {
public:
    /// asynchronously load from file
    static MeshSetup FromFile(const class Locator& loc, Id placeholder=Id::InvalidId()) {
        MeshSetup setup;
        setup.VertexUsage = Usage::Immutable;
        setup.IndexUsage = Usage::Immutable;
        setup.Locator = loc;
        setup.Placeholder = placeholder;
        setup.setupFromFile = true;
        return setup;
    }
    /// setup from from data in memory
    static MeshSetup FromData(Usage::Code vertexUsage=Usage::Immutable, Usage::Code indexUsage=Usage::Immutable) {
        MeshSetup setup;
        setup.VertexUsage = vertexUsage;
        setup.IndexUsage = indexUsage;
        setup.setupFromData = true;
        return setup;
    }
    /// setup from data in memory with blueprint
    static MeshSetup FromData(const MeshSetup& blueprint) {
        MeshSetup setup(blueprint);
        setup.setupFromData = true;
        return setup;
    }
    /// setup empty mesh (mostly for dynamic streaming)
    static MeshSetup Empty(int numVertices, Usage::Code vertexUsage, IndexType::Code indexType=IndexType::None, int numIndices=0, Usage::Code indexUsage=Usage::InvalidUsage) {
        o_assert_dbg(numVertices > 0);
        MeshSetup setup;
        setup.setupEmpty = true;
        setup.VertexUsage = vertexUsage;
        setup.IndexUsage = indexUsage;
        setup.NumVertices = numVertices;
        setup.NumIndices = numIndices;
        setup.IndicesType = indexType;
        setup.DataVertexOffset = InvalidIndex;
        setup.DataIndexOffset = InvalidIndex;
        return setup;
    }
    /// setup a fullscreen quad mesh
    static MeshSetup FullScreenQuad(bool flipV=false) {
        MeshSetup setup;
        setup.setupFullScreenQuad = true;
        setup.FullScreenQuadFlipV = flipV;
        setup.Layout.Add(VertexAttr::Position, VertexFormat::Float3);
        setup.Layout.Add(VertexAttr::TexCoord0, VertexFormat::Float2);
        return setup;
    }
    /// check if should load asynchronously
    bool ShouldSetupFromFile() const {
        return this->setupFromFile;
    }
    /// check if should setup from data in memory
    bool ShouldSetupFromData() const {
        return this->setupFromData;
    }
    /// check if should setup empty mesh
    bool ShouldSetupEmpty() const {
        return this->setupEmpty;
    }
    /// check if should setup fullscreen quad mesh
    bool ShouldSetupFullScreenQuad() const {
        return this->setupFullScreenQuad;
    }
    /// add a primitive group (required for CreateEmpty)
    void AddPrimitiveGroup(const PrimitiveGroup& primGroup) {
        o_assert(this->setupEmpty || this->setupFromData);
        o_assert(this->numPrimGroups < GfxConfig::MaxNumPrimGroups);
        this->primGroups[this->numPrimGroups++] = primGroup;
    }
    /// get number of primitive groups
    int NumPrimitiveGroups() const {
        return this->numPrimGroups;
    }
    /// get primitive group at index
    const class PrimitiveGroup& PrimitiveGroup(int index) const {
        o_assert_range(index, GfxConfig::MaxNumPrimGroups);
        return this->primGroups[index];
    }
    /// vertex-data usage
    Usage::Code VertexUsage = Usage::InvalidUsage;
    /// index-data usage
    Usage::Code IndexUsage = Usage::InvalidUsage;
    /// vertex layout
    VertexLayout Layout;
    /// number of vertices
    int NumVertices = 0;
    /// number of indices
    int NumIndices = 0;
    /// index type 
    IndexType::Code IndicesType = IndexType::None;
    /// flip v coordinates for fullscreen quad (so that origin is top-left)
    bool FullScreenQuadFlipV = false;
    /// resource locator
    class Locator Locator = Locator::NonShared();
    /// placeholder Id
    Id Placeholder;
    /// vertex data byte offset in data (default: 0, set to InvalidIndex if no vertex data provided)
    int DataVertexOffset = 0;
    /// index data byte offset in data (default: InvalidIndex, no index data provided)
    int DataIndexOffset = 0;
private:
    int numPrimGroups = 0;
    class PrimitiveGroup primGroups[GfxConfig::MaxNumPrimGroups];
    bool setupFromFile = false;
    bool setupFromData = false;
    bool setupEmpty = false;
    bool setupFullScreenQuad = false;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::PipelineSetup
    @ingroup Gfx
    @brief setup object for pipeline resources
*/
class PipelineSetup {
public:
    /// construct from shader
    static PipelineSetup FromShader(const Id& shd) {
        o_assert_dbg(shd.IsValid());
        PipelineSetup setup;
        setup.Shader = shd;
        return setup;
    }
    /// construct from vertex layout and shader
    static PipelineSetup FromLayoutAndShader(const VertexLayout& layout, const Id& shd) {
        o_assert_dbg(!layout.Empty() && shd.IsValid());
        PipelineSetup setup;
        setup.Layouts[0] = layout;
        setup.Shader = shd;
        return setup;
    }
    /// resource locator
    class Locator Locator = Locator::NonShared();
    /// blend state (GLES3.0 doesn't allow separate MRT blend state
    class BlendState BlendState;
    /// blend color
    glm::vec4 BlendColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    /// depth-stencil state
    class DepthStencilState DepthStencilState;
    /// rasterizer state
    class RasterizerState RasterizerState;
    /// input vertex layouts (one per mesh slot)
    StaticArray<VertexLayout, GfxConfig::MaxNumInputMeshes> Layouts;
    /// primitive type 
    PrimitiveType::Code PrimType = PrimitiveType::Triangles;
    /// shader 
    Id Shader;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::RenderPassSetup
    @ingroup Gfx
    @brief setup attributes for RenderPass resource
*/
class RenderPassSetup {
public:
    /// construct from single render target textures, and option depth-stencil texture
    static RenderPassSetup From(Id colorTexture, Id depthStencilTexture=Id::InvalidId()) {
        RenderPassSetup setup;
        setup.ColorAttachments[0].Texture = colorTexture;
        setup.DepthStencilAttachment.Texture = depthStencilTexture;
        return setup;
    }
    /// construct from MRT render target textures, and option depth-stencil texture
    static RenderPassSetup From(std::initializer_list<Id> colorTextures, Id depthStencilTexture=Id::InvalidId()) {
        RenderPassSetup setup;
        int i = 0;
        for (const auto& id : colorTextures) {
            setup.ColorAttachments[i++].Texture = id;
        }
        setup.DepthStencilAttachment.Texture = depthStencilTexture;
        return setup;
    }
    /// resource locator
    class Locator Locator = Locator::NonShared();
    /// 1..N color attachments
    struct ColorAttachment {
        Id Texture;
        uint16_t Level = 0;     ///< mipmap-level
        uint16_t Layer = 0;     ///< layer (for 3D- or 2D-array-textures)
        uint16_t Face = 0;      ///< cubemap face
        RenderPassLoadAction::Code LoadAction = RenderPassLoadAction::Clear;
        glm::vec4 ClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    };
    StaticArray<ColorAttachment, GfxConfig::MaxNumColorAttachments> ColorAttachments;
    /// optional depth-stencil attachment
    struct DepthStencilAttachment {
        Id Texture;
        RenderPassLoadAction::Code LoadAction = RenderPassLoadAction::Clear;
        float ClearDepth = 1.0f;
        uint8_t ClearStencil = 0;
    };
    struct DepthStencilAttachment DepthStencilAttachment;
    /// what to do in Gfx::EndPass (preserve or drop content, MSAA resolve)
    RenderPassStoreAction::Code StoreAction = RenderPassStoreAction::Store;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::ShaderSetup
    @ingroup Gfx
    @brief setup class for shaders
*/
class ShaderSetup {
public:
    /// default constructor
    ShaderSetup() { }
    /// construct with resource locator
    ShaderSetup(const Locator& loc) : Locator(loc) { }
    /// the resource locator
    class Locator Locator = Locator::NonShared();
    /// set shader program from vertex- and fragment-shader sources
    void SetProgramFromSources(ShaderLang::Code slang, const String& vsSource, const String& fsSource) {
        o_assert_dbg(vsSource.IsValid() && fsSource.IsValid());
        this->program.vsSources[slang] = vsSource;
        this->program.fsSources[slang] = fsSource;
    }
    /// set shader program from precompiled shader byte code
    void SetProgramFromByteCode(ShaderLang::Code slang, const uint8_t* vsByteCode, uint32_t vsNumBytes, const uint8_t* fsByteCode, uint32_t fsNumBytes) {
        o_assert_dbg(vsByteCode && (vsNumBytes > 0));
        o_assert_dbg(fsByteCode && (fsNumBytes > 0));
        this->program.vsByteCode[slang].ptr = vsByteCode;
        this->program.vsByteCode[slang].size = vsNumBytes;
        this->program.fsByteCode[slang].ptr = fsByteCode;
        this->program.fsByteCode[slang].size = fsNumBytes;
    }
    /// set shader program from a metal-style shader library
    void SetProgramFromLibrary(ShaderLang::Code slang, const char* vsFunc, const char* fsFunc) {
        o_assert_dbg(ShaderLang::Metal == slang);
        o_assert_dbg(vsFunc && fsFunc);
        this->program.vsFuncs[slang] = vsFunc;
        this->program.fsFuncs[slang] = fsFunc;
    }
    /// set vertex shader input layout
    void SetInputLayout(const VertexLayout& vsInputLayout) {
        this->program.vsInputLayout = vsInputLayout;
    }
    /// add a uniform block
    void AddUniformBlock(const StringAtom& name, const UniformBlockLayout& layout, ShaderStage::Code bindStage, int32_t bindSlot) {
        o_assert_dbg(name.IsValid());
        o_assert_dbg(!layout.Empty());
        o_assert_dbg(0 != layout.TypeHash);
        o_assert_dbg(bindSlot >= 0);
        uniformBlockEntry& entry = this->uniformBlocks[this->numUniformBlocks++];
        entry.name = name;
        entry.layout = layout;
        entry.bindStage = bindStage;
        entry.bindSlot = bindSlot;
    }
    /// add a texture block
    void AddTextureBlock(const StringAtom& name, const TextureBlockLayout& layout, ShaderStage::Code bindStage) {
        o_assert_dbg(name.IsValid());
        textureBlockEntry& entry = this->textureBlocks[this->numTextureBlocks++];
        entry.name = name;
        entry.layout = layout;
        entry.bindStage = bindStage;
    }
    /// set metal-style library byte code
    void SetLibraryByteCode(ShaderLang::Code slang, const uint8_t* byteCode, uint32_t numBytes) {
        o_assert_dbg(ShaderLang::Metal == slang);
        o_assert_dbg(nullptr != byteCode);
        o_assert_dbg(numBytes > 0);
        this->libraryByteCode = byteCode;
        this->libraryByteCodeSize = numBytes;
    }
    /// get metal-style library byte code
    void LibraryByteCode(ShaderLang::Code slang, const void*& outPtr, uint32_t& outSize) const {
        o_assert_dbg(ShaderLang::Metal == slang);
        outPtr = this->libraryByteCode;
        outSize = this->libraryByteCodeSize;
    }
    /// get the vertex shader input layout
    const VertexLayout& InputLayout() const {
        return this->program.vsInputLayout;
    }
    /// get program vertex shader source (only valid if setup from sources)
    const String& VertexShaderSource(ShaderLang::Code slang) const {
        return this->program.vsSources[slang];
    }
    /// get program fragment shader source (only valid if setup from sources)
    const String& FragmentShaderSource(ShaderLang::Code slang) const {
        return this->program.fsSources[slang];
    }
    /// get program vertex shader byte code, returns nullptr if no byte code exists
    void VertexShaderByteCode(ShaderLang::Code slang, const void*& outPtr, uint32_t& outSize) const {
        outPtr = this->program.vsByteCode[slang].ptr;
        outSize = this->program.vsByteCode[slang].size;
    }
    /// get program fragment shader byte code, returns nullptr if no byte code exists
    void FragmentShaderByteCode(ShaderLang::Code slang, const void*& outPtr, uint32_t& outSize) const {
        outPtr = this->program.fsByteCode[slang].ptr;
        outSize = this->program.fsByteCode[slang].size;
    }
    /// get vertex shader name (if using metal-style shader library
    const String& VertexShaderFunc(ShaderLang::Code slang) const {
        o_assert_dbg(ShaderLang::Metal == slang);
        return this->program.vsFuncs[slang];
    }
    /// get fragment shader name (if using metal-style shader library
    const String& FragmentShaderFunc(ShaderLang::Code slang) const {
        o_assert_dbg(ShaderLang::Metal == slang);
        return this->program.fsFuncs[slang];
    }
    /// get number of uniform blocks
    int NumUniformBlocks() const {
        return this->numUniformBlocks;
    }
    /// find uniform block index by bind stage and slot (return InvalidIndex if not found)
    int UniformBlockIndexByStageAndSlot(ShaderStage::Code bindStage, int bindSlot) const {
        for (int i = 0; i < this->numUniformBlocks; i++) {
            const auto& entry = this->uniformBlocks[i];
            if ((entry.bindStage == bindStage) && (entry.bindSlot == bindSlot)) {
                return i;
            }
        }
        return InvalidIndex;
    }
    /// get uniform block name at index
    const StringAtom& UniformBlockName(int index) const {
        return this->uniformBlocks[index].name;
    }
    /// get uniform block layout at index
    const class UniformBlockLayout& UniformBlockLayout(int index) const {
        return this->uniformBlocks[index].layout;
    }
    /// get uniform block shader stage at index
    ShaderStage::Code UniformBlockBindStage(int index) const {
        return this->uniformBlocks[index].bindStage;
    }
    /// get uniform block bind slot at index
    int UniformBlockBindSlot(int index) const {
        return this->uniformBlocks[index].bindSlot;
    }
    /// get number of texture blocks
    int NumTextureBlocks() const {
        return this->numTextureBlocks;
    }
    /// find texture block index by bind stage (return InvalidIndex if not found)
    int TextureBlockIndexByStage(ShaderStage::Code bindStage) const {
        for (int i = 0; i < this->numTextureBlocks; i++) {
            const auto& entry = this->textureBlocks[i];
            if (entry.bindStage == bindStage) {
                return i;
            }
        }
        return InvalidIndex;
    }
    /// get texture block name at index
    const StringAtom& TextureBlockName(int index) const {
        return this->textureBlocks[index].name;
    }
    /// get texture block layout at index
    const class TextureBlockLayout& TextureBlockLayout(int index) const {
        return this->textureBlocks[index].layout;
    }
    /// get texture block shader stage at index
    ShaderStage::Code TextureBlockBindStage(int index) const {
        return this->textureBlocks[index].bindStage;
    }
private:
    struct programEntry {
        StaticArray<String, ShaderLang::NumShaderLangs> vsSources;
        StaticArray<String, ShaderLang::NumShaderLangs> fsSources;
        StaticArray<String, ShaderLang::NumShaderLangs> vsFuncs;
        StaticArray<String, ShaderLang::NumShaderLangs> fsFuncs;
        struct byteCodeEntry {
            const void* ptr = nullptr;
            uint32_t size = 0;
        };
        StaticArray<byteCodeEntry, ShaderLang::NumShaderLangs> vsByteCode;
        StaticArray<byteCodeEntry, ShaderLang::NumShaderLangs> fsByteCode;
        VertexLayout vsInputLayout;
    };
    struct uniformBlockEntry {
        StringAtom name;
        class UniformBlockLayout layout;
        ShaderStage::Code bindStage = ShaderStage::InvalidShaderStage;
        int bindSlot = InvalidIndex;
    };
    struct textureBlockEntry {
        StringAtom name;
        class TextureBlockLayout layout;
        ShaderStage::Code bindStage = ShaderStage::InvalidShaderStage;
    };
    static const int MaxNumUniformBlocks = ShaderStage::NumShaderStages * GfxConfig::MaxNumUniformBlocksPerStage;
    int libraryByteCodeSize = 0;
    const void* libraryByteCode = nullptr;
    programEntry program;
    int numUniformBlocks = 0;
    StaticArray<uniformBlockEntry, MaxNumUniformBlocks> uniformBlocks;
    int numTextureBlocks = 0;
    StaticArray<textureBlockEntry, ShaderStage::NumShaderStages> textureBlocks;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::TextureSetup
    @ingroup Gfx
    @brief setup object for textures and render targets
*/
class TextureSetup {
public:
    /// asynchronously load from file
    static TextureSetup FromFile(const Locator& loc, Id placeholder=Id::InvalidId()) {
        TextureSetup setup;
        setup.setupFromFile = true;
        setup.Locator = loc;
        setup.Placeholder = placeholder;
        return setup;
    }
    /// asynchronously load from file
    static TextureSetup FromFile(const Locator& loc, const TextureSetup& blueprint=TextureSetup(), Id placeholder=Id::InvalidId()) {
        TextureSetup setup(blueprint);
        setup.setupFromFile = true;
        setup.Locator = loc;
        setup.Placeholder = placeholder;
        return setup;
    }
    /// setup 2D texture from raw pixel data
    static TextureSetup FromPixelData2D(int w, int h, int numMipMaps, PixelFormat::Code fmt, const TextureSetup& blueprint=TextureSetup()) {
        o_assert_dbg((w > 0) && (h > 0));
        o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
        o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));
        TextureSetup setup(blueprint);
        setup.setupFromPixelData = true;
        setup.Type = TextureType::Texture2D;
        setup.Width = w;
        setup.Height = h;
        setup.NumMipMaps = numMipMaps;
        setup.ColorFormat = fmt;
        setup.ImageData.NumFaces = 1;
        setup.ImageData.NumMipMaps = numMipMaps;
        return setup;
    }
    /// setup cube texture from raw pixel data
    static TextureSetup FromPixelDataCube(int w, int h, int numMipMaps, PixelFormat::Code fmt, const TextureSetup& blueprint=TextureSetup()) {
        o_assert_dbg((w > 0) && (h > 0));
        o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
        o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));
        TextureSetup setup(blueprint);
        setup.setupFromPixelData = true;
        setup.Type = TextureType::TextureCube;
        setup.Width = w;
        setup.Height = h;
        setup.NumMipMaps = numMipMaps;
        setup.ColorFormat = fmt;
        setup.ImageData.NumFaces = 6;
        setup.ImageData.NumMipMaps = numMipMaps;
        return setup;
    }
    //// setup 3D texture from raw pixel data
    static TextureSetup FromPixelData3D(int w, int h, int d, int numMipMaps, PixelFormat::Code fmt, const TextureSetup& blueprint=TextureSetup()) {
        o_assert_dbg((w > 0) && (h > 0) && (d > 0));
        o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
        o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));
        TextureSetup setup(blueprint);
        setup.setupFromPixelData = true;
        setup.Type = TextureType::Texture3D;
        setup.Width = w;
        setup.Height = h;
        setup.Depth = d;
        setup.NumMipMaps = numMipMaps;
        setup.ColorFormat = fmt;
        setup.ImageData.NumFaces = 1;
        setup.ImageData.NumMipMaps = numMipMaps;
        return setup;
    }
    /// setup array texture from raw pixel data
    static TextureSetup FromPixelDataArray(int w, int h, int layers, int numMipMaps, PixelFormat::Code fmt, const TextureSetup& blueprint=TextureSetup()) {
        o_assert_dbg((w > 0) && (h > 0) && (layers > 0));
        o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
        o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));
        TextureSetup setup(blueprint);
        setup.setupFromPixelData = true;
        setup.Type = TextureType::TextureArray;
        setup.Width = w;
        setup.Height = h;
        setup.Depth = layers;
        setup.NumMipMaps = numMipMaps;
        setup.ColorFormat = fmt;
        setup.ImageData.NumFaces = 1;
        setup.ImageData.NumMipMaps = numMipMaps;
        return setup;
    }
    /// setup empty 2D texture
    static TextureSetup Empty2D(int w, int h, int numMipMaps, PixelFormat::Code fmt, Usage::Code usage, const TextureSetup& blueprint=TextureSetup()) {
        o_assert_dbg((w > 0) && (h > 0));
        o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
        o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));
        TextureSetup setup(blueprint);
        setup.setupEmpty = true;
        setup.Type = TextureType::Texture2D;
        setup.Width = w;
        setup.Height = h;
        setup.NumMipMaps = numMipMaps;
        setup.ColorFormat = fmt;
        setup.TextureUsage = usage;
        return setup;
    }
    /// setup empty cube texture
    static TextureSetup EmptyCube(int w, int h, int numMipMaps, PixelFormat::Code fmt, Usage::Code usage, const TextureSetup& blueprint=TextureSetup()) {
        o_assert_dbg((w > 0) && (h > 0));
        o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
        o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));
        TextureSetup setup(blueprint);
        setup.setupEmpty = true;
        setup.Type = TextureType::TextureCube;
        setup.Width = w;
        setup.Height = h;
        setup.NumMipMaps = numMipMaps;
        setup.ColorFormat = fmt;
        setup.TextureUsage = usage;
        return setup;
    }
    /// setup empty 3D texture
    static TextureSetup Empty3D(int w, int h, int d, int numMipMaps, PixelFormat::Code fmt, Usage::Code usage, const TextureSetup& blueprint=TextureSetup()) {
        o_assert_dbg((w > 0) && (h > 0) && (d > 0));
        o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
        o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));
        TextureSetup setup(blueprint);
        setup.setupEmpty = true;
        setup.Type = TextureType::Texture3D;
        setup.Width = w;
        setup.Height = h;
        setup.Depth = d;
        setup.NumMipMaps = numMipMaps;
        setup.ColorFormat = fmt;
        setup.TextureUsage = usage;
        return setup;
    }
    /// setup empty array texture
    static TextureSetup EmptyArray(int w, int h, int layers, int numMipMaps, PixelFormat::Code fmt, Usage::Code usage, const TextureSetup& blueprint=TextureSetup()) {
        o_assert_dbg((w > 0) && (h > 0) && (layers > 0));
        o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
        o_assert_dbg((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));
        TextureSetup setup(blueprint);
        setup.setupEmpty = true;
        setup.Type = TextureType::TextureArray;
        setup.Width = w;
        setup.Height = h;
        setup.Depth = layers;
        setup.NumMipMaps = numMipMaps;
        setup.ColorFormat = fmt;
        setup.TextureUsage = usage;
        return setup;
    }
    /// setup as 2D render target
    static TextureSetup RenderTarget2D(int w, int h, PixelFormat::Code colorFmt=PixelFormat::RGBA8, PixelFormat::Code depthFmt=PixelFormat::None) {
        o_assert_dbg((w > 0) && (h > 0));
        TextureSetup setup;
        setup.Type = TextureType::Texture2D;
        setup.RenderTarget = true;
        setup.Width = w;
        setup.Height = h;
        setup.ColorFormat = colorFmt;
        setup.DepthFormat = depthFmt;
        setup.Sampler.WrapU = TextureWrapMode::ClampToEdge;
        setup.Sampler.WrapV = TextureWrapMode::ClampToEdge;
        return setup;
    }
    /// setup as cube render target
    static TextureSetup RenderTargetCube(int w, int h, PixelFormat::Code colorFmt=PixelFormat::RGBA8, PixelFormat::Code depthFmt=PixelFormat::None) {
        o_assert_dbg((w > 0) && (h > 0));
        TextureSetup setup;
        setup.Type = TextureType::TextureCube;
        setup.RenderTarget = true;
        setup.Width = w;
        setup.Height = h;
        setup.ColorFormat = colorFmt;
        setup.DepthFormat = depthFmt;
        setup.Sampler.WrapU = TextureWrapMode::ClampToEdge;
        setup.Sampler.WrapV = TextureWrapMode::ClampToEdge;
        return setup;
    }
    /// setup as 3D render target
    static TextureSetup RenderTarget3D(int w, int h, int d, PixelFormat::Code colorFmt=PixelFormat::RGBA8, PixelFormat::Code depthFmt=PixelFormat::None) {
        o_assert_dbg((w > 0) && (h > 0));
        TextureSetup setup;
        setup.Type = TextureType::Texture3D;
        setup.RenderTarget = true;
        setup.Width = w;
        setup.Height = h;
        setup.Depth = d;
        setup.ColorFormat = colorFmt;
        setup.DepthFormat = depthFmt;
        setup.Sampler.WrapU = TextureWrapMode::ClampToEdge;
        setup.Sampler.WrapV = TextureWrapMode::ClampToEdge;
        return setup;
    }
    /// setup as array render target
    static TextureSetup RenderTargetArray(int w, int h, int layers, PixelFormat::Code colorFmt=PixelFormat::RGBA8, PixelFormat::Code depthFmt=PixelFormat::None) {
        o_assert_dbg((w > 0) && (h > 0));
        TextureSetup setup;
        setup.Type = TextureType::TextureArray;
        setup.RenderTarget = true;
        setup.Width = w;
        setup.Height = h;
        setup.Depth = layers;
        setup.ColorFormat = colorFmt;
        setup.DepthFormat = depthFmt;
        setup.Sampler.WrapU = TextureWrapMode::ClampToEdge;
        setup.Sampler.WrapV = TextureWrapMode::ClampToEdge;
        return setup;
    }
    /// setup texture from existing native texture(s) (needs GfxFeature::NativeTexture)
    static TextureSetup FromNativeTexture(int w, int h, int numMipMaps, TextureType::Code type, PixelFormat::Code fmt, Usage::Code usage, intptr_t h0, intptr_t h1=0) {
        o_assert_dbg((w > 0) && (h > 0));
        o_assert_dbg(PixelFormat::IsValidTextureColorFormat(fmt));
        o_assert((numMipMaps > 0) && (numMipMaps < GfxConfig::MaxNumTextureMipMaps));
        o_assert_dbg(h0 != 0);
        TextureSetup setup;
        setup.setupFromNativeHandle = true;
        setup.Type = type;
        setup.Width = w;
        setup.Height = h;
        setup.NumMipMaps = numMipMaps;
        setup.ColorFormat = fmt;
        setup.TextureUsage = usage;
        setup.NativeHandle[0] = h0;
        setup.NativeHandle[1] = h1;
        return setup;
    }
    /// return true if texture should be setup from a file
    bool ShouldSetupFromFile() const {
        return this->setupFromFile;
    }
    /// return true if texture should be setup from raw pixel data
    bool ShouldSetupFromPixelData() const {
        return this->setupFromPixelData;
    }
    /// return true if texture should be setup from native texture handles
    bool ShouldSetupFromNativeTexture() const {
        return this->setupFromNativeHandle;
    }
    /// return true if texture should be created empty
    bool ShouldSetupEmpty() const {
        return this->setupEmpty;
    }
    /// return true if render target has depth
    bool HasDepth() const {
        return this->DepthFormat != PixelFormat::InvalidPixelFormat;
    }
    /// intended usage
    Usage::Code TextureUsage = Usage::Immutable;
    /// texture type
    TextureType::Code Type = TextureType::Texture2D;
    /// use as render target?
    bool RenderTarget = false;
    /// width in pixels
    int Width = 1;
    /// height in pixels
    int Height = 1;
    /// depth/layers in pixels (for 3D and Array textures)
    int Depth = 1;
    /// number of mipmaps (default is 1, only for FromPixelData)
    int NumMipMaps = 1;
    /// the color pixel format
    PixelFormat::Code ColorFormat = PixelFormat::RGBA8;
    /// the depth pixel format (only if render target, PixelFormat::None if render target should not have depth buffer)
    PixelFormat::Code DepthFormat = PixelFormat::None;
    /// MSAA samples (2, 4, 8... no MSAA: 1), check MSAARenderTargets feature availability!
    int SampleCount = 1;
    /// sampler state
    SamplerState Sampler;
    /// resource locator
    class Locator Locator = Locator::NonShared();
    /// resource placeholder
    Id Placeholder;
    /// optional: native texture handle (only on platforms which support GfxFeature::NativeTextures)
    static const int MaxNumNativeHandles = 2;
    StaticArray<intptr_t, MaxNumNativeHandles> NativeHandle;
    /// optional image surface offsets and sizes
    ImageDataAttrs ImageData;
    /// default constructor 
    TextureSetup() {
        NativeHandle.Fill(0);
    }
private:
    bool setupFromFile = false;
    bool setupFromPixelData = false;
    bool setupFromNativeHandle = false;
    bool setupEmpty = false;
    bool hasMipMaps = false;
};
    
} // namespace Oryol
