#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11Shader
    @ingroup _priv
    @brief D3D11 implementation of shader
*/
#include "Gfx/Resource/shaderBase.h"
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
    void addUniformBlockEntry(ID3D11Buffer* cb, ShaderType::Code bindShaderStage, int32 bindSlotIndex);
    /// get number of uniform block entries
    int32 getNumUniformBlockEntries() const;
    /// get uniform block at index
    ID3D11Buffer* getUniformBlockEntryAtIndex(int32 index, ShaderType::Code& outBindShaderStage, int32& outBindSlotIndex) const;

private:
    struct programEntry {
        programEntry() : mask(0), vertexShader(nullptr), pixelShader(nullptr) {};

        uint32 mask;
        ID3D11VertexShader* vertexShader;
        ID3D11PixelShader* pixelShader;
    };
    struct ubEntry {
        ubEntry() : 
            constantBuffer(nullptr), 
            bindShaderStage(ShaderType::InvalidShaderType),
            bindSlotIndex(InvalidIndex) {};

        // NOTE: the constantBuffer pointer can be 0 if
        // the associated uniform-block has only texture params!
        ID3D11Buffer* constantBuffer;
        ShaderType::Code bindShaderStage;
        int32 bindSlotIndex;
    };
    int32 numPrograms;
    StaticArray<programEntry, GfxConfig::MaxNumBundlePrograms> programEntries;
    int32 numUniformBlockEntries;
    StaticArray<ubEntry, GfxConfig::MaxNumUniformBlocks> uniformBlockEntries;
};

//------------------------------------------------------------------------------
inline int32
d3d11Shader::getNumUniformBlockEntries() const {
    return this->numUniformBlockEntries;
}

//------------------------------------------------------------------------------
inline ID3D11Buffer*
d3d11Shader::getUniformBlockEntryAtIndex(int32 index, ShaderType::Code& outBindShaderStage, int32& outBindSlotIndex) const {
    const ubEntry& entry = this->uniformBlockEntries[index];
    outBindShaderStage = entry.bindShaderStage;
    outBindSlotIndex = entry.bindSlotIndex;
    return entry.constantBuffer;
}

} // namespace _priv
} // namespace Oryol
