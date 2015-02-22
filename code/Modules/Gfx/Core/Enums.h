#pragma once
//------------------------------------------------------------------------------
/**
    @file Gfx/Core/Enums.h
    @brief Gfx module enums
*/
#include "Core/Types.h"
#include "Core/Assertion.h"
#if ORYOL_OPENGL
#include "Gfx/gl/glEnums.h"
#endif

namespace Oryol {

//------------------------------------------------------------------------------
/**
    @class Oryol::IndexType
    @ingroup Gfx
    @brief selects 16- or 32-bit indices
*/
#if ORYOL_OPENGL
class IndexType : public _priv::glIndexType {
#endif
public:
    /// get byte size of index type
    static int32 ByteSize(IndexType::Code c) {
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
    @brief RGBA/Depth/Stencil channel bits and cominations
 */
class PixelChannel {
public:
    typedef uint32 Mask;
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
    /// pixel format enum
    enum Code {
        RGBA8,          ///< 32-bit wide, 4 channels @ 8-bit
        RGB8,           ///< 24-bit wide, 3 channels @ 8-bit
        RGBA4,          ///< 16-bit wide, 4 channels @ 4-bit
        R5G6B5,         ///< 16-bit wide, 3 channels @ 5/6/5 bits
        R5G5B5A1,       ///< 16-bit wide, 4 channels @ 1-bit alpha, 5-bit rgb
        RGBA32F,        ///< 128-bit wide, 4 channel @ 32-bit float
        RGBA16F,        ///< 64-bit wide, 4 channel @ 16-bit float
        L8,             ///< 8-bit wide, single channel
        DXT1,           ///< DXT1 compressed format
        DXT3,           ///< DXT3 compressed format
        DXT5,           ///< DXT5 compressed format
        D16,            ///< 16-bit depth
        D32,            ///< 32-bit depth
        D24S8,          ///< 24-bit depth, 8-bit stencil
        PVRTC2,         ///< PVRTC2 compressed format
        PVRTC4,         ///< PVRTC4 compressed format
        
        NumPixelFormats,            ///< number of pixel formats
        InvalidPixelFormat,         ///< invalid pixel format value
        None = InvalidPixelFormat,  ///< special "none" type
    };

    /// return true for valid render target color formats
    static bool IsValidRenderTargetColorFormat(Code c) {
        switch (c) {
            case RGBA8:
            case RGB8:
            case RGBA4:
            case R5G6B5:
            case R5G5B5A1:
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
            case D16:
            case D32:
            case D24S8:
                return true;
            default:
                return false;
        }
    }
    /// return true for valid color texture formats
    static bool IsValidTextureColorFormat(Code c) {
        switch (c) {
            case D16:
            case D32:
            case D24S8:
                return false;
            default:
                return true;
        }
    }
    /// return true for valid depth texture formats
    static bool IsValidTextureDepthFormat(Code c) {
        switch (c) {
            case D16:
            case D32:
            case D24S8:
                return true;
            default:
                return false;
        }
    }
    /// test if the pixel format is a pure depth format (not a depth/stencil format)
    static bool IsDepthFormat(Code c) {
        return (D16 == c) || (D32 == c);
    }
    /// test if the pixel format is a depth/stencil format
    static bool IsDepthStencilFormat(Code c) {
        return D24S8 == c;
    }
    /// return true if the pixel format is a compressed format
    static bool IsCompressedFormat(Code c) {
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
    /// get byte size of pixel format
    static int32 ByteSize(Code c) {
        switch (c) {
            case RGBA32F:
                return 16;
            case RGBA16F:
                return 8;
            case RGBA8:
                return 4;
            case RGB8:
                return 3;
            case R5G6B5:
            case R5G5B5A1:
            case RGBA4:
                return 2;
            case L8:
                return 1;
            case D16:
                return 2;
            case D32:
            case D24S8:
                return 4;
            default:
                o_error("PixelFormat::ByteSize(): cannot get byte size for compressed format!\n");
                return 0;
        }
    }
    /// get number of bits in a pixel format channel (only for non-compressed formats!)
    static int8 NumBits(Code pixelFormat, PixelChannel::Bits channel) {
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
            case D16:
                if (PixelChannel::Depth == channel) {
                    return 16;
                }
                break;
            case D32:
                if (PixelChannel::Depth == channel) {
                    return 32;
                }
                break;
            case D24S8:
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
};

//------------------------------------------------------------------------------
/**
    @class Oryol::PrimitiveType
    @ingroup Gfx
    @brief primitive type enum (triangle strips, lists, etc...)
*/
#if ORYOL_OPENGL
class PrimitiveType : public _priv::glPrimitiveType { };
#endif

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
        Shader,             ///< a vertex shader
        ProgramBundle,      ///< shader program bundle
        ConstantBlock,      ///< block constant shader uniforms
        DrawState,          ///< draw state resource
        
        NumResourceTypes,
        InvalidResourceType = 0xFFFF,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::ShaderType
    @ingroup Gfx
    @brief shader types (vertex shader, fragment shader)
*/
#if ORYOL_OPENGL
class ShaderType : public _priv::glShaderType { };
#endif

//------------------------------------------------------------------------------
/**
    @class Oryol::TextureFilterMode
    @ingroup Gfx
    @brief texture sampling filter mode
*/
#if ORYOL_OPENGL
class TextureFilterMode  : public _priv::glTextureFilterMode { };
#endif
   
//------------------------------------------------------------------------------
/**
    @class Oryol::TextureType
    @ingroup Gfx
    @brief texture type (2D, 3D, Cube)
*/
#if ORYOL_OPENGL
class TextureType : public _priv::glTextureType { };
#endif

//------------------------------------------------------------------------------
/**
    @class Oryol::TextureWrapMode
    @ingroup Gfx
    @brief texture coordinate wrapping modes
*/
#if ORYOL_OPENGL
class TextureWrapMode : public _priv::glTextureWrapMode { };
#endif

//------------------------------------------------------------------------------
/**
    @class Oryol::Usage
    @ingroup Gfx
    @brief graphics resource usage types
*/
#if ORYOL_OPENGL
class Usage : public _priv::glUsage { };
#endif

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
    enum Code {
        Position = 0,   ///> "position"
        Normal,         ///> "normal"
        TexCoord0,      ///> "texcoord0"
        TexCoord1,      ///> "texcoord1"
        TexCoord2,      ///> "texcoord2"
        TexCoord3,      ///> "texcoord3"
        Tangent,        ///> "tangent
        Binormal,       ///> "binormal"
        Weights,        ///> "weights" (skin weights)
        Indices,        ///> "indices" (skin indices)
        Color0,         ///> "color0"
        Color1,         ///> "color1"
        Instance0,      ///> "instance0"
        Instance1,      ///> "instance1"
        Instance2,      ///> "instance2"
        Instance3,      ///> "instance3"
        
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
    @see VertexComponent, VertexLayout
*/
class VertexFormat {
public:
    /// format enum (don't change order, and append to end!)
    enum Code {
        Float,          ///< single component float, expanded to (x, 0, 0, 1)
        Float2,         ///< 2-component float, expanded to (x, y, 0, 1)
        Float3,         ///< 3-component float, expanded to (x, y, z, 1)
        Float4,         ///< 4-component float
        Byte4,          ///< 4-component float (-128.0f..+127.0f) mapped to byte (-128..+127)
        Byte4N,         ///< 4-component float (-1.0f..+1.0f) mapped to byte (-128..+127)
        UByte4,         ///< 4-component float (0.0f..255.0f) mapped to byte (0..255)
        UByte4N,        ///< 4-component float (0.0f..+1.0) mapped to byte (-128..+127)
        Short2,         ///< 2-component float (-32768.0f..+32767.0f) mapped to short (-32768..+32768)
        Short2N,        ///< 2-component float (-1.0f..+1.0f) mapped to short (-32768..+32768)
        Short4,         ///< 4-component float (-32768.0f..+32767.0f) mapped to short (-32768..+32768)
        Short4N,        ///< 4-component float (-1.0f..+1.0f) mapped to short (-32768..+32768)
        
        NumVertexFormats,       ///< number of vertex formats
        InvalidVertexFormat,    ///< the invalid vertex format value
    };
    
    /// get the byte size of a vertex format code
    static int32 ByteSize(Code c) {
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
        TextureCompressionDXT,      ///< GPU supports DXT compressed textures
        TextureCompressionPVRTC,    ///< GPU supports PVRTC compressed textures
        TextureCompressionATC,      ///< GPU supports ATC compressed textures
        TextureCompressionETC2,     ///< GPU supports ETC2 compressed textures (OpenGLES3)
        TextureFloat,               ///< support for float textures
        TextureHalfFloat,           ///< support for half-float textures
        Instancing,                 ///< supports hardware-instanced rendering
        
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
    enum Code {
        Front = 0,
        Back,
        Both,
        
        NumFaceCodes,
        NumSides = 2,
        InvalidFace = -1
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::CompareFunc
    @ingroup Gfx
    @brief comparison modes for depth and stencil state
*/
class CompareFunc {
public:
    enum Code {
        Never = 0,
        Less,
        Equal,
        LessEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always,
        
        NumCompareFuncs,
        InvalidCompareFunc,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::StencilOperation
    @ingroup Gfx
    @brief stencil operations
*/
class StencilOp {
public:
    enum Code {
        Keep,
        Zero,
        Replace,
        IncrClamp,
        DecrClamp,
        Invert,
        IncrWrap,
        DecrWrap,
        
        NumStencilOperations,
        InvalidStencilOperation,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::BlendFactor
    @ingroup Gfx
    @brief blending factors
*/
class BlendFactor {
public:
    enum Code {
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
        
        NumBlendFactors,
        InvalidBlendFactor,
    };
};
 
//------------------------------------------------------------------------------
/**
    @class Oryol::BlendOperation
    @ingroup Gfx
    @brief blending operations
*/
class BlendOperation {
public:
    enum Code {
        Add = 0,
        Subtract,
        ReverseSubtract,
        
        NumBlendOperations,
        InvalidBlendOperation,
    };
};

} // namespace Oryol
