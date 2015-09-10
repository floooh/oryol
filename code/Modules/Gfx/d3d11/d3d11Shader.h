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

    /// add vs/ps pair with mask
    int32 addShaders(uint32 mask, ID3D11VertexShader* vs, ID3D11PixelShader* ps);
    /// get program index by mask, return InvalidIndex if not found
    int32 getProgIndexByMask(uint32 mask) const;
    /// get vertex shader by mask, reutrn nullptr if not found
    ID3D11VertexShader* getVertexShaderByMask(uint32 mask) const;
    /// get pixel shader by mask, return nullptr if not found
    ID3D11PixelShader* getPixelShaderByMask(uint32 mask) const;

    /// get number of programs
    int32 getNumPrograms() const;
    /// get vertex shader at program index
    ID3D11VertexShader* getVertexShaderAtIndex(int32 index) const;
    /// get pixel shader at program index
    ID3D11PixelShader* getPixelShaderAtIndex(int32 index) const;

    /// add a uniform block entry
    void addUniformBlockEntry(ShaderStage::Code bindStage, int32 bindSlot, ID3D11Buffer* cb);
    /// get uniform block constant buffer at bind stage and slot (can return nullptr)
    ID3D11Buffer* getConstantBuffer(ShaderStage::Code bindStage, int32 bindSlot) const;

private:
    struct programEntry {
        programEntry() : mask(0), vertexShader(nullptr), pixelShader(nullptr) {};

        uint32 mask;
        ID3D11VertexShader* vertexShader;
        ID3D11PixelShader* pixelShader;
    };
    static const int32 NumConstantBuffers = ShaderStage::NumShaderStages * GfxConfig::MaxNumUniformBlocksPerStage;
    int32 numPrograms;
    StaticArray<programEntry, GfxConfig::MaxNumBundlePrograms> programEntries;
    StaticArray<ID3D11Buffer*, NumConstantBuffers> constantBuffers;
};

//------------------------------------------------------------------------------
inline ID3D11Buffer*
d3d11Shader::getConstantBuffer(ShaderStage::Code bindStage, int32 bindSlot) const {
    const int32 cbIndex = (ShaderStage::NumShaderStages * bindStage) + bindSlot;
    return this->constantBuffers[cbIndex];
}

} // namespace _priv
} // namespace Oryol
