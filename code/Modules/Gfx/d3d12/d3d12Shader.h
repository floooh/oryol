#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12Shader
    @ingroup _priv
    @brief D3D12 implementation of shader
*/
#include "Gfx/Resource/shaderBase.h"
#include "Core/Containers/StaticArray.h"
#include "Gfx/d3d12/d3d12_decl.h"

namespace Oryol {
namespace _priv {

class d3d12Shader : public shaderBase {
public:
    /// destructor
    ~d3d12Shader();
    /// clear the object
    void Clear();

    /// pointer to and size of compiled shader data
    struct shaderBlob {
        const void* ptr = nullptr;
        uint32_t size = 0;
    };
    shaderBlob vertexShader;
    shaderBlob pixelShader;
};

} // namespace _priv
} // namespace Oryol
