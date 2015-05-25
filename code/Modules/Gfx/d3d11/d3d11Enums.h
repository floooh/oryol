#pragma once
//------------------------------------------------------------------------------
/**
    @file Gfx/d3d11/d3d11Enums
    @ingroup _priv
    @brief Gfx module enums mapped to their D3D11 values
*/
#include "Core/Types.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11IndexType
    @ingroup _priv
*/
class d3d11IndexType {
public:
    enum Code {
        None = 0,
        Index16 = 57,       // DXGI_FORMAT_R16_UINT
        Index32 = 42,       // DXGI_FORMAT_R32_UINT

        NumIndexTypes = 3,
        InvalidIndexType = 0xFFFFFFFF
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11PrimitiveType
    @ingroup _priv
*/
class d3d11PrimitiveType {
public:
    /// primitive type enum (don't change order, append to end!)
    enum Code {
        Points = 1,         // D3D11_PRIMITIVE_TOPOLOGY_POINTLIST
        Lines = 2,          // D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
        LineStrip = 3,      // D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP 
        Triangles = 4,      // D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
        TriangleStrip = 5,  // D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP

        NumPrimitiveTypes = 5,
        InvalidPrimitiveType = 0xFFFFFFFF,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11ShaderType
    @ingroup _priv
*/
class d3d11ShaderType {
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
    @class Oryol::_priv::d3d11TextureFilterMode
    @ingroup _priv

    In D3D11, all texture filter mode combinations are defined with a
    single value.
*/
class d3d11TextureFilterMode {
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
    @class Oryol::_priv::d3d11TextureType
    @ingroup _priv
*/
class d3d11TextureType {
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
    @class Oryol::_priv::d3d11TextureWrapMode
    @ingroup _priv
*/
class d3d11TextureWrapMode {
public:
    /// wrap modes
    enum Code {
        ClampToEdge = 3,        // D3D11_TEXTURE_ADDRESS_CLAMP
        Repeat = 1,             // D3D11_TEXTURE_ADDRESS_WRAP
        MirroredRepeat = 2,     // D3D11_TEXTURE_ADDRESS_MIRROR

        NumTextureWrapModes = 3,
        InvalidTextureWrapMode = 0xFFFFFFFF,
    };
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11Usage
    @ingroup _priv

    These don't have direct equivalents in D3D11
*/
class d3d11Usage {
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