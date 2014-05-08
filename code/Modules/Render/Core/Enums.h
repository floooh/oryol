#pragma once
//------------------------------------------------------------------------------
/**
    @file Render/Core/Enums.h
    @brief Render module enums
*/
#include "Core/Types.h"
#include "Core/Macros.h"

namespace Oryol {
namespace Render {

//------------------------------------------------------------------------------
/**
    @class Oryol::Render::IndexType
    @brief selects 16- or 32-bit indices
*/
class IndexType {
public:
    /// type enum
    enum Code {
        None = 0,           ///< mesh has no indices
        Index16 = 1,        ///< 16-bit indices
        Index32 = 2,        ///< 32-bit indices
        
        NumIndexTypes,      ///< number of index types
        InvalidIndexType,   ///< the invalid index type value
    };
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
class PrimitiveType {
public:
    /// primitive type enum (don't change order, append to end!)
    enum Code {
        Points = 0,             ///< point list
        LineStrip,              ///< line strip
        LineLoop,               ///< closed line loop
        Lines,                  ///< line list
        TriangleStrip,          ///< triangle strip
        TriangleFan,            ///< triangle fan
        Triangles,              ///< triangle list
        
        NumPrimitiveTypes,      ///< number of primitive types
        InvalidPrimitiveType,   ///< invalid primitive type value
    };
};

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
        StateBlock,         ///< block of render states
        ConstantBlock,      ///< block constant shader uniforms
        
        NumResourceTypes,
        InvalidResourceType = 0xFFFF,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::Render::ShaderType
    @brief shader types (vertex shader, fragment shader)
*/
class ShaderType {
public:
    /// shader types enum
    enum Code {
        VertexShader,       ///< vertex shader
        FragmentShader,     ///< fragment shader
        
        NumShaderTypes,     ///< number of shader types
        InvalidShaderType,  ///< invalid shader type
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::Render::State
    @brief render states
*/
class State {
public:
    /// these are the render state codes (keys)
    enum Code {
        FrontFace,              ///< what's the front side of a poly (CW, CCW)
        CullFaceEnabled,        ///< face-culling enabled (True, False)
        CullFace,               ///< cull face mode (Front, Back, FrontAndBack
        
        DepthOffsetEnabled,     ///< whether depth offset is enabled (True, False)
        DepthOffset,            ///< depth offset values (float factor, float unit)
        
        ScissorTestEnabled,     ///< enable/disable scissor test (True, False)
        ScissorRect,            ///< the scissor rectangle (int left, int bottom, int left, int right)
        
        StencilTestEnabled,     ///< whether stencil testing is enabled (True, False)
        StencilFunc,            ///< the stencil func (func=Never/Always/Less/LessEqual/GreaterEqual/Greater/NotEqual, int ref, uint mask)
        StencilFuncSeparate,    ///< separate stencil funcs for front/back (face=Front/Back/FrontAndBack, func=Never/Always/Less/LessEqual/GreaterEqual/Greater/NotEqual,int ref, uint mask)
        StencilOp,              ///< stencil operation (sfail,dpfail,dppass=Keep/Zero/Replace/Incr/Decr/Invert/IncrWrap/DecrWrap)
        StencilOpSeparate,      ///< separate stencil operations (face=Front/Back/FrontAndBack, sfail,dpfail,dppass=Keep/Zero/Replace/Incr/Decr/Invert/IncrWrap/DecrWrap)
        
        DepthTestEnabled,       ///< whether depth test is enabled (True, False)
        DepthFunc,              ///< depth test function (Never/Always/Less/LessEqual/GreaterEqual/Greater/NotEqual)
        
        BlendEnabled,           ///< whether alpha blending is enabled (True, False)
        BlendEquation,          ///< combined RGB/A blend mode (Add, Subtract, ReverseSubtract)
        BlendEquationSeparate,  ///< separate RGBA + A blend mode (rgb=Add/Subtract/ReverseSubtract, a=Add/Subtract/ReverseSubtract)
        BlendFunc,              ///< RGBA blend function (src,dst=Zero/One/SrcColor/InvSrcColor/DstColor/InvDstColor/SrcAlpha/InvSrcAlpha/DstAlpha/InvDstAlpha/ConstColor/InvConstColor/ConstAlpha/InvConstAlpha)
        BlendFuncSeparate,      ///< separate RGB+A blend func, (srcRGB,dstRGB,srcA,dstA, see above)
        BlendColor,             ///< color for ConstColor/ConstAlpha (4x float)
        
        DitherEnabled,          ///< whether dithering is on (True, False)
        ColorMask,              ///< color channel write mask (4x True|False)
        DepthMask,              ///< whether depth writes are enabled (True, False)
        StencilMask,            ///< stencil write bit mask (uint)
        StencilMaskSeparate,    ///< stencil write bit masks for polygon-side (face=Front|Back|FrontAndBack, uint mask)
        
        ClearColor,             ///< the clear color values (4x float)
        ClearDepth,             ///< the clear depth value (float)
        ClearStencil,           ///< the clear stencil value (int)
        
        ViewPort,               ///< the current viewport (int x, int y, int w, int h)
        DepthRange,             ///< the current depth range (float n, float f)
        
        NumStateCodes,
        InvalidStateCode
    };
    
    /// these are the render state values
    enum Value {
        Undefined,      ///< state is not set
        Zero,           ///< stencil func / blend factor: zero, or replace with zerp
        One,            ///< blend factor: set to one
        
        CW,             ///< for FrontFace state
        CCW,            ///< for FrontFace state
        
        Front,          ///< polygon front side
        Back,           ///< polygon back side
        FrontAndBack,   ///< polygon front and back
        
        Never,          ///< stencil/depth func: never pass
        Always,         ///< stencil/depth func: always pass
        Less,           ///< stencil/depth func: pass if less-then
        LessEqual,      ///< stencil/depth func: pass if less-or-equal
        GreaterEqual,   ///< stencil/depth func: pass if greater-or-equal
        Greater,        ///< stencil/dpeth func: pass if greater
        NotEqual,       ///< stencil/depth func: pass if not equal
        
        Keep,           ///< stencil func: keep value
        // Zero,        ///< stencil func: set value to zero
        Replace,        ///< stencil func: replace with reference value
        Incr,           ///< stencil func: incremenet with saturation
        Decr,           ///< stencil func: decrement with saturation
        Invert,         ///< stencil func: bitwise invert
        IncrWrap,       ///< stencil func: increment with wrap-around
        DecrWrap,       ///< stencil func: decrement with wrap-around
        
        // Zero             ///< blend factor: zero
        // One              ///< blend factor: one
        SrcColor,           ///< blend factor: src-color
        InvSrcColor,        ///< blend factor: one-minus-src-color
        DstColor,           ///< blend factor: dst-color
        InvDstColor,        ///< blend factor: one-minus-dst-color
        SrcAlpha,           ///< blend factor: src-alpha
        InvSrcAlpha,        ///< blend factor: one-minus-src-alpha
        DstAlpha,           ///< blend factor: dst-alpha
        InvDstAlpha,        ///< blend factor: one-minus-dst-alpha
        ConstColor,         ///< blend factor: constant-color
        InvConstColor,      ///< blend factor: one-minus-constant-color
        ConstAlpha,         ///< blend factor: constant-alpha
        InvConstAlpha,      ///< blend factor: one-minus-constant-alpha
        SrcAlphaSaturate,   ///< blend factor: (f,f,f), f=min(As,1-Ad)
        
        NumStateValues,
        InvalidStateValue
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::Render::TextureFilterMode
    @brief texture sampling filter mode
*/
class TextureFilterMode {
public:
    /// filtering modes
    enum Code {
        Nearest,                    ///< nearest (point) filtering, no mipmapping
        Linear,                     ///< linear filtering, no mipmapping
        NearestMipmapNearest,       ///< nearest texel, nearest mipmap
        NearestMipmapLinear,        ///< nearest texel, linear mipmap filtering
        LinearMipmapNearest,        ///< linear texel filtering, nearest mipmap filtering
        LinearMipmapLinear,         ///< linear texel filtering, nearest mipmap filtering
        
        NumTextureFilterModes,      ///< number of texture filtering modes
        InvalidTextureFilterMode    ///< the invalid texture filtering mode value
    };
};
   
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::TextureType
    @brief texture type (2D, 3D, Cube)
*/
class TextureType {
public:
    /// texture type enum
    enum Code {
        Texture2D,              ///< 2D texture
        Texture3D,              ///< 3D texture
        TextureCube,            ///< cube map texture
        
        NumTextureTypes,        ///< number of texture types
        InvalidTextureType,     ///< the invalid texture type value
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::Render::TextureWrapMode
    @brief texture coordinate wrapping modes
*/
class TextureWrapMode {
public:
    /// wrap modes
    enum Code {
        ClampToEdge,                ///< clamp uv coords to [0,1]
        Repeat,                     ///< repeat (wrap-around) uv coords
        MirroredRepeat,             ///< mirror-repeat uv coords
        
        NumTextureWrapModes,        ///< number of texture wrap modes
        InvalidTextureWrapMode,     ///< the invalid texture wrap mode value
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::Render::Usage
    @brief graphics resource usage types
*/
class Usage {
public:
    /// usage enum
    enum Code {
        Immutable,      ///< resource is immutable, can only be initialized
        DynamicWrite,   ///< dynamic resource, infrequently written by CPU
        DynamicStream,  ///< dynamic resource, frequently written by CPU
        
        NumUsages,      ///< number of resource usages
        InvalidUsage    ///< the invalid usage value
    };
};

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
        Custom0,        ///> "custom0"
        Custom1,        ///> "custom1"
        Custom2,        ///> "custom2"
        Custom3,        ///> "custom3"
        
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
            case Custom0:   return "custom0";
            case Custom1:   return "custom1";
            case Custom2:   return "custom2";
            case Custom3:   return "custom3";
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
    
} // namespace Render
} // namespace Oryol
