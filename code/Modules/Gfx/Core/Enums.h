#pragma once
//------------------------------------------------------------------------------
/**
    @file Gfx/Core/Enums.h
    @brief Gfx module enums
*/
#include "Core/Types.h"
#include "Core/Assertion.h"
#include "Resource/Id.h"

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
        Texture3D,
        TextureCube,

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
                return 0;
        }
    }
};

//------------------------------------------------------------------------------
/**
    @class Oryol::VertexFormat
    @ingroup Gfx
    @brief vertex component formats
    @see VertexLayout
    
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
        Int10_2,        ///< 4-component packed signed 10-bit XYZ, 2-bit W (-512..+511)
        Int10_2N,       ///< 4-component packed, normalized 10-bit XYZ, 2-bit W (-1.0 .. +1.0)
        UInt10_2,       ///< 4-component packed unsigned 10-bit XYZ, 2-bit W (0..1024)
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
            case Int10_2:
            case Int10_2N:
            case UInt10_2:
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
        GLSL150,        ///< OpenGL 3.0
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
        VertexCapture,              ///< vertex shader output capture (aka transform feedback / stream output)

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

} // namespace Oryol
