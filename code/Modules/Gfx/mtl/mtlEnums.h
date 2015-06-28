#pragma once
//------------------------------------------------------------------------------
/**
    @file Gfx/mtl/mtlEnums.h
    @ingroup _priv
    @brief Gfx module enums mapped to their Metal values
*/
#include "Core/Types.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlIndexType
    @ingroup _priv
*/
class mtlIndexType {
public:
    enum Code {
        None = -1,
        Index16 = 0,        // MTLIndexTypeUInt16
        Index32 = 1,        // MTLIndexTypeUInt32

        NumIndexTypes = 3,
        InvalidIndexType = 0xFFFFFFFF
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlPrimitiveType
    @ingroup _priv
*/
class mtlPrimitiveType {
public:
    /// primitive type enum (don't change order, append to end!)
    enum Code {
        Points = 0,                 // MTLPrimitiveTypePoint
        Lines = 1,                  // MTLPrimitiveTypeLine
        LineStrip = 2,              // MTLPrimitiveTypeLineStrip
        Triangles = 3,              // MTLPrimitiveTypeTriangle
        TriangleStrip = 4,          // MTLPrimitiveTypeTriangleStrip

        NumPrimitiveTypes = 5,
        InvalidPrimitiveType = 0xFFFFFFFF,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlShaderType
    @ingroup _priv
*/
class mtlShaderType {
public:
    /// shader types enum
    enum Code {
        VertexShader,
        FragmentShader,

        NumShaderTypes = 2,
        InvalidShaderType = 0xFFFFFFFF,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlTextureFilterMode
    @ingroup _priv
*/
class mtlTextureFilterMode {
public:
    /// filtering modes
    enum Code {
        Nearest,
        Linear,
        NearestMipmapNearest,
        NearestMipmapLinear,
        LinearMipmapNearest,
        LinearMipmapLinear,
        
        NumTextureFilterModes = 6,
        InvalidTextureFilterMode = 0xFFFFFFFF,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlTextureType
    @ingroup _priv
*/
class mtlTextureType {
public:
    /// texture type enum
    enum Code {
        Texture2D,
        Texture3D,
        TextureCube,
        
        NumTextureTypes = 3,
        InvalidTextureType = 0xFFFFFFFF,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlTextureWrapMode
    @ingroup _priv
*/
class mtlTextureWrapMode {
public:
    /// wrap modes
    enum Code {
        ClampToEdge = 0,            // MTLSamplerAddressModeClampToEdge
        Repeat = 2,                 // MTLSamplerAddressModeRepeat
        MirroredRepeat = 3,         // MTLSamplerAddressModeMirrorRepeat
                
        NumTextureWrapModes = 3,
        InvalidTextureWrapMode = 0xFFFFFFFF,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlUsage
    @ingroup _priv
*/
class mtlUsage {
public:
    /// usage enum
    enum Code {
        Immutable,
        Static,
        Dynamic,
        Stream,
        
        NumUsages = 4,
        InvalidUsage = 0xFFFFFFFF,
    };
};


} // namespace _priv
} // namespace Oryol