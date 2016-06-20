#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11Shader
    @ingroup _priv
    @brief D3D11 implementation of shader
*/
#include "Gfx/Resource/shaderBase.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/Core/GfxConfig.h"
#include "Core/Containers/StaticArray.h"
#include "Gfx/d3d11/d3d11_decl.h"

namespace Oryol {
namespace _priv {

class d3d11Shader : public shaderBase {
public:
    /// constructor
    d3d11Shader();
    /// destructor
    ~d3d11Shader();

    /// clear the object
    void Clear();

    /// the D3D11 vertex shader
    ID3D11VertexShader* d3d11VertexShader;
    /// the D3D1 pixel shader
    ID3D11PixelShader* d3d11PixelShader;
    /// add a uniform block entry
    void addUniformBlockEntry(ShaderStage::Code bindStage, int bindSlot, ID3D11Buffer* cb);
    /// get uniform block constant buffer at bind stage and slot (can return nullptr)
    ID3D11Buffer* getConstantBuffer(ShaderStage::Code bindStage, int bindSlot) const;

private:
    static const int NumConstantBuffers = ShaderStage::NumShaderStages * GfxConfig::MaxNumUniformBlocksPerStage;
    StaticArray<ID3D11Buffer*, NumConstantBuffers> constantBuffers;
};

//------------------------------------------------------------------------------
inline ID3D11Buffer*
d3d11Shader::getConstantBuffer(ShaderStage::Code bindStage, int bindSlot) const {
    const int cbIndex = (GfxConfig::MaxNumUniformBlocksPerStage * bindStage) + bindSlot;
    return this->constantBuffers[cbIndex];
}

} // namespace _priv
} // namespace Oryol
