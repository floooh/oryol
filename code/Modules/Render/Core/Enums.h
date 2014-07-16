#pragma once
//------------------------------------------------------------------------------
/**
    @file Render/Core/Enums.h
    @brief Render module enums
*/
#include "Core/Types.h"
#include "Core/Assert.h"
#if ORYOL_OPENGL
#include "Render/gl/glEnums.h"
#endif

namespace Oryol {
namespace Render {

//------------------------------------------------------------------------------
/**
    @class Oryol::Render::IndexType
    @brief selects 16- or 32-bit indices
*/
#if ORYOL_OPENGL
class IndexType : public glIndexType {
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
    @class Oryol::Render::PixelFormat
    @brief enum of pixel formats
*/
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

    /// return true for valid render target color formats
    static bool IsValidRenderTargetColorFormat(Code c) {
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
            case R8G8B8A8:
                return 4;
            case R8G8B8:
                return 3;
            case R5G6B5:
            case R5G5B5A1:
            case R4G4B4A4:
                return 2;
            case L8:
                return 1;
            case D16:
                return 2;
            case D32:
            case D24S8:
            case R32F:
                return 4;
            default:
                o_error("PixelFormat::ByteSize(): cannot get byte size for compressed format!\n");
                return 0;
        }
    }
    /// get number of bits in a pixel format channel (only for non-compressed formats!)
    static int8 NumBits(Code pixelFormat, Channel channel) {
        switch (pixelFormat) {
            case R8G8B8A8:
                if ((Red == channel) || (Green == channel) || (Blue == channel) || (Alpha == channel)) {
                    return 8;
                }
                break;
            case R8G8B8:
                if ((Red == channel) || (Green == channel) || (Blue == channel)) {
                    return 8;
                }
                break;
            case R5G6B5:
                if ((Red == channel) || (Blue == channel)) {
                    return 5;
                }
                else if (Green == channel) {
                    return 6;
                }
                break;
            case R5G5B5A1:
                if ((Red == channel) || (Green == channel) || (Blue == channel)) {
                    return 5;
                }
                else if (Alpha == channel) {
                    return 1;
                }
                break;
            case R4G4B4A4:
                if ((Red == channel) || (Green == channel) || (Blue == channel) || (Alpha == channel)) {
                    return 4;
                }
                break;
            case L8:
                if (Red == channel) {
                    return 8;
                }
                break;
            case D16:
                if (Depth == channel) {
                    return 16;
                }
                break;
            case D32:
                if (Depth == channel) {
                    return 32;
                }
                break;
            case D24S8:
                if (Depth == channel) {
                    return 24;
                }
                else if (Stencil == channel) {
                    return 8;
                }
                break;
            case R32F:
                if (Red == channel) {
                    return 32;
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
    @class Oryol::Render::PrimitiveType
    @brief primitive type enum (triangle strips, lists, etc...)
*/
#if ORYOL_OPENGL
class PrimitiveType : public glPrimitiveType { };
#endif

//------------------------------------------------------------------------------
/**
    @class Oryol::Render::ResourceType
    @brief Render subsystem resource types
 
    These types are used for the type in Resource::Id for Render module
    resources.
*/
class ResourceType {
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
    @class Oryol::Render::ShaderType
    @brief shader types (vertex shader, fragment shader)
*/
#if ORYOL_OPENGL
class ShaderType : public glShaderType { };
#endif

//------------------------------------------------------------------------------
/**
    @class Oryol::Render::State
    @brief render states
*/
#if ORYOL_OPENGL
class State {
#endif
public:
    /// these are the render state codes (keys)
    enum Code {
        DepthOffset,            ///< depth offset values (float factor, float unit)
        ScissorRect,            ///< the scissor rectangle (int left, int bottom, int left, int right)
        BlendColor,             ///< color for ConstColor/ConstAlpha (4x float)
        ClearColor,             ///< the clear color values (4x float)
        ClearDepth,             ///< the clear depth value (float)
        ClearStencil,           ///< the clear stencil value (int)
        ViewPort,               ///< the current viewport (int x, int y, int w, int h)
        DepthRange,             ///< the current depth range (float n, float f)
        
        NumStateCodes,
        InvalidStateCode
    };
    
    struct Variant {
        Variant() : i(0) {};
        union {
            int32 i;
            float32 f;
        };
    };
    struct Vector {
        Variant val[4];
    };
    
    static const uint32 U = 0; // undefined
    static const uint32 F = 2; // float32
    static const uint32 I = 3; // int32
    
    enum Signature {
        Void = 0,
        F0 = F,                             ///< 1 float32
        F0_F1 = F|F<<3,                     ///< 2 float32
        F0_F1_F2_F3 = F|F<<3|F<<6|F<<9,     ///< 4 float32
        I0 = I,                             ///< 1 int32
        I0_I1 = I|I<<3,                     ///< 2 int32
        I0_I1_I2_I3 = I|I<<3|I<<6|I<<9,     ///< 4 int32
    };
    
    struct Object {
        Code state;
        Signature sig;
        Vector vec;
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::Render::TextureFilterMode
    @brief texture sampling filter mode
*/
#if ORYOL_OPENGL
class TextureFilterMode  : public glTextureFilterMode { };
#endif
   
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::TextureType
    @brief texture type (2D, 3D, Cube)
*/
#if ORYOL_OPENGL
class TextureType : public glTextureType { };
#endif

//------------------------------------------------------------------------------
/**
    @class Oryol::Render::TextureWrapMode
    @brief texture coordinate wrapping modes
*/
#if ORYOL_OPENGL
class TextureWrapMode : public glTextureWrapMode { };
#endif

//------------------------------------------------------------------------------
/**
    @class Oryol::Render::Usage
    @brief graphics resource usage types
*/
#if ORYOL_OPENGL
class Usage : public glUsage { };
#endif

//------------------------------------------------------------------------------
/**
    @class Oryol::Render::VertexAttr
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
    @class Oryol::Render::VertexFormat
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
    @class Oryol::Render::ShaderLang
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
    @class Oryol::Render::Feature
    @brief optional rendering features
*/
class Feature {
public:
    enum Code {
        TextureCompressionDXT,      ///< GPU supports DXT compressed textures
        TextureCompressionPVRTC,    ///< GPU supports PVRTC compressed textures
        TextureCompressionATC,      ///< GPU supports ATC compressed textures
        Instancing,                 ///< supports hardware-instanced rendering
        
        NumFeatures,
        InvalidFeature
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::Render::Face
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
    @class Oryol::Render::CompareFunc
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
    @class Oryol::Render::StencilOperation
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
    @class Oryol::Render::BlendFactor
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
    @class Oryol::Render::BlendOperation
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

//------------------------------------------------------------------------------
/**
    @class Oryol::Render::ColorWriteMask
    @brief color write masking for blend state
*/
class ColorWriteMask {
public:
    enum Code {
        None    = 0,
        
        R   = (1<<3),
        G   = (1<<2),
        B   = (1<<1),
        A   = (1<<0),
        
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
        
        All  = R|G|B|A,
        RGBA = R|G|B|A
    };
};

} // namespace Render
} // namespace Oryol
