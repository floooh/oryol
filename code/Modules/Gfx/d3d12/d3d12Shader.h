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
    /// pointer to and size of compiled shader data
    struct shaderBlob {
        const void* ptr = nullptr;
        uint32 size = 0;
    };

    /// constructor
    d3d12Shader();
    /// destructor
    ~d3d12Shader();

    /// clear the object
    void Clear();

    /// add vs/ps pair with selection mask
    int32 addShaders(uint32 mask, const shaderBlob& vs, const shaderBlob& ps);
    /// get vertex shader by selection mask, return nullptr if not found
    const shaderBlob* getVertexShaderByMask(uint32 mask) const;
    /// get pixel shader by selection mask, return nullptr if not found
    const shaderBlob* getPixelShaderByMask(uint32 mask) const;

private:
    struct programEntry {
        programEntry() : mask(0) { };
        uint32 mask;
        shaderBlob vertexShader;
        shaderBlob pixelShader;
    };
    int32 numPrograms;
    StaticArray<programEntry, GfxConfig::MaxNumBundlePrograms> programEntries;
};

} // namespace _priv
} // namespace Oryol
