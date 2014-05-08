#pragma once
//------------------------------------------------------------------------------
/**
    @file Render/gl/glEnums.h
    @brief Render module enums mapped to their GL values
*/
#include "Core/Types.h"

namespace Oryol {
namespace Render {

//------------------------------------------------------------------------------
/**
    @class Oryol::Render::glIndexType
 */
class glIndexType {
public:
    enum Code {
        None = 0,
        Index16 = 0x1403,   // GL_UNSIGNED_SHORT
        Index32 = 0x1405,   // GL_UNSIGNED_INT
        
        NumIndexTypes = 3,
        InvalidIndexType = 0xFFFFFFFF
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::Render::glPrimitiveType
*/
class glPrimitiveType {
public:
    /// primitive type enum (don't change order, append to end!)
    enum Code {
        Points = 0x0000,            // GL_POINTS
        Lines = 0x0001,             // GL_LINES
        LineLoop = 0x0002,          // GL_LINE_LOOP
        LineStrip = 0x0003,         // GL_LINE_STRIP
        Triangles = 0x0004,         // GL_TRIANGLES
        TriangleStrip = 0x0005,     // GL_TRIANGLE_STRIP
        TriangleFan = 0x0006,       // GL_TRIANGLE_FAN
        
        NumPrimitiveTypes = 7,
        InvalidPrimitiveType = 0xFFFFFFFFF,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::Render::glShaderType
*/
class glShaderType {
public:
    /// shader types enum
    enum Code {
        VertexShader = 0x8B31,      // GL_VERTEX_SHADER
        FragmentShader = 0x8B30,    // GL_FRAGMENT_SHADER
        
        NumShaderTypes = 2,
        InvalidShaderType = 0xFFFFFFFF,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::Render::glTextureFilterMode
*/
class glTextureFilterMode {
public:
    /// filtering modes
    enum Code {
        Nearest = 0x2600,                   // GL_NEAREST
        Linear = 0x2601,                    // GL_LINEAR
        NearestMipmapNearest = 0x2700,      // GL_NEAREST_MIPMAP_NEAREST
        NearestMipmapLinear = 0x2702,       // GL_NEAREST_MIPMAP_LINEAR
        LinearMipmapNearest = 0x2701,       // GL_LINEAR_MIPMAP_NEAREST
        LinearMipmapLinear = 0x2703,        // GL_LINEAR_MIPMAP_LINEAR
        
        NumTextureFilterModes = 6,
        InvalidTextureFilterMode = 0xFFFFFFFF,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::Render::glTextureType
*/
class glTextureType {
public:
    /// texture type enum
    enum Code {
        Texture2D = 0x0DE1,         // GL_TEXTURE_2D
        Texture3D = 0x806F,         // GL_TEXTURE_3D
        TextureCube = 0x8513,       // GL_TEXTURE_CUBE_MAP
        
        NumTextureTypes = 3,
        InvalidTextureType = 0xFFFFFFFF,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::Render::glTextureWrapMode
*/
class glTextureWrapMode {
public:
    /// wrap modes
    enum Code {
        ClampToEdge = 0x812F,       // GL_CLAMP_TO_EDGE
        Repeat = 0x2901,            // GL_REPEAT
        MirroredRepeat = 0x8370,    // GL_MIRRORED_REPEAT
                
        NumTextureWrapModes = 3,
        InvalidTextureWrapMode = 0xFFFFFFFF,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::Render::glUsage
*/
class glUsage {
public:
    /// usage enum
    enum Code {
        Immutable = 0x88E4,         // GL_STATIC_DRAW
        DynamicWrite = 0x88E8,      // GL_DYNAMIC_DRAW
        DynamicStream = 0x88E0,     // GL_STREAM_DRAW
        
        NumUsages = 3,
        InvalidUsage = 0xFFFFFFFF,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::Render::glState
*/
class glState {
public:
    /// these are the render state values
    enum Value {
        Undefined = -1,         ///< state is not set
        Zero = 0,               ///< GL_ZERO, stencil func / blend factor: zero, or replace with zero
        One = 1,                ///< GL_ONE, blend factor: set to one
        
        CW = 0x0900,            ///< GL_CW, for FrontFace state
        CCW = 0x0901,           ///< GL_CCW, for FrontFace state
        
        Front = 0x0404,         ///< GL_FRONT, polygon front side
        Back = 0x0405,          ///< GL_BACK, polygon back side
        FrontAndBack = 0x0408,  ///< GL_FRONT_AND_BACK, polygon front and back
        
        Never = 0x0200,         ///< GL_NEVER, stencil/depth func: never pass
        Always = 0x0207,        ///< GL_ALWAYS, stencil/depth func: always pass
        Less = 0x0201,          ///< GL_LESS, stencil/depth func: pass if less-then
        LessEqual = 0x0203,     ///< GL_LEQUAL, stencil/depth func: pass if less-or-equal
        GreaterEqual = 0x0206,  ///< GL_GEQUAL, stencil/depth func: pass if greater-or-equal
        Greater = 0x0204,       ///< GL_GREATER, stencil/depth func: pass if greater
        Equal = 0x0202,         ///< GL_EQUAL, stencil/depth func: pass if equal
        NotEqual = 0x0205,      ///< GL_NOTEQUAL, stencil/depth func: pass if not equal
        
        Keep = 0x1E00,          ///< GL_KEEP, stencil func: keep value
        // Zero,                ///< GL_ZERO, stencil func: set value to zero
        Replace = 0x1E01,       ///< GL_REPLACE, stencil func: replace with reference value
        Incr = 0x1E02,          ///< GL_INCR, stencil func: incremenet with saturation
        Decr = 0x1E03,          ///< GL_DECR, stencil func: decrement with saturation
        Invert = 0x150A,        ///< GL_INVERT, stencil func: bitwise invert
        IncrWrap = 0x8507,      ///< GL_INCR_WRAP, stencil func: increment with wrap-around
        DecrWrap = 0x8508,      ///< GL_DECR_WRAP, stencil func: decrement with wrap-around
        
        // Zero                     ///< GL_ZERO, blend factor: zero
        // One                      ///< GL_ONE, blend factor: one
        SrcColor = 0x0300,          ///< GL_SRC_COLOR, blend factor: src-color
        InvSrcColor = 0x0301,       ///< GL_ONE_MINUS_SRC_COLOR, blend factor: one-minus-src-color
        DstColor = 0x0306,          ///< GL_DST_COLOR, blend factor: dst-color
        InvDstColor = 0x0307,       ///< GL_ONE_MINUS_DST_COLOR, blend factor: one-minus-dst-color
        SrcAlpha = 0x0302,          ///< GL_SRC_ALPHA, blend factor: src-alpha
        InvSrcAlpha = 0x0303,       ///< GL_ONE_MINUS_SRC_ALPHA, blend factor: one-minus-src-alpha
        DstAlpha = 0x0304,          ///< GL_DST_ALPHA, blend factor: dst-alpha
        InvDstAlpha = 0x0305,       ///< GL_ONE_MINUS_DST_ALPHA, blend factor: one-minus-dst-alpha
        ConstColor = 0x8001,        ///< GL_CONSTANT_COLOR, blend factor: constant-color
        InvConstColor = 0x8002,     ///< GL_ONE_MINUS_CONSTANT_COLOR, blend factor: one-minus-constant-color
        ConstAlpha = 0x8003,        ///< GL_CONSTANT_ALPHA, blend factor: constant-alpha
        InvConstAlpha = 0x8004,     ///< GL_ONE_MINUS_CONSTANT_ALPHA, blend factor: one-minus-constant-alpha
        SrcAlphaSaturate = 0x0308,  ///< GL_SRC_ALPHA_SATURATE, blend factor: (f,f,f), f=min(As,1-Ad)
        
        InvalidStateValue = 0xFFFFFFFF
    };
};

} // namespace Render
} // namespace Oryol
