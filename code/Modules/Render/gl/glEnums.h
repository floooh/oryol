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
        Immutable = 0x88E4,     // GL_STATIC_DRAW
        Dynamic = 0x88E8,       // GL_DYNAMIC_DRAW
        Stream = 0x88E0,        // GL_STREAM_DRAW
        
        NumUsages = 3,
        InvalidUsage = 0xFFFFFFFF,
    };
};

} // namespace Render
} // namespace Oryol
