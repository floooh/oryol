#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11ProgramBundle
    @ingroup _priv
    @brief D3D11 implementation of program bundle

    NOTE: Currently, constant buffers are part of the d3d11ProgramBundle
    object, so basically 'per-shader' not 'per-drawstate'. This does
    not allow to 'bake' per-material constant buffers, or share
    constant buffers between shaders.
*/
#include "Gfx/Resource/programBundleBase.h"
#include "Core/Containers/StaticArray.h"
#include "Gfx/d3d11/d3d11_decl.h"

namespace Oryol {
namespace _priv {

class d3d11ProgramBundle : public programBundleBase {
public:
    /// constructor
    d3d11ProgramBundle();
    /// destructor
    ~d3d11ProgramBundle();

    /// clear the object
    void Clear();

    /// add vs/ps pair with selection mask
    int32 addShaders(uint32 mask, ID3D11VertexShader* vs, ID3D11PixelShader* ps);
    /// select a vs/ps pair
    bool select(uint32 mask);
    /// get the current selection mask
    uint32 getSelectionMask() const;
    /// get the current selection index
    uint32 getSelectionIndex() const;
    /// get the currently selected vertex shader
    ID3D11VertexShader* getSelectedVertexShader() const;
    /// get the currently selected pixel shader
    ID3D11PixelShader* getSelectedPixelShader() const;

    /// get number of programs
    int32 getNumPrograms() const;
    /// get vertex shader at program index
    ID3D11VertexShader* getVertexShaderAt(int32 index) const;
    /// get pixel shader at program index
    ID3D11PixelShader* getPixelShaderAt(int32 index) const;

    /// add a constant buffer
    int32 addConstantBuffer(ID3D11Buffer* cb, ShaderType::Code bindShaderStage, int32 bindSlotIndex);
    /// get number of constant buffers
    int32 getNumConstantBuffers() const;
    /// get constant buffer at index
    ID3D11Buffer* getConstantBufferAt(int32 index, ShaderType::Code& outBindShaderStage, int32& outBindSlotIndex) const;

private:
    struct programEntry {
        programEntry() : mask(0), vertexShader(nullptr), pixelShader(nullptr) {};

        uint32 mask;
        ID3D11VertexShader* vertexShader;
        ID3D11PixelShader* pixelShader;
    };
    struct cbEntry {
        cbEntry() : 
            constantBuffer(nullptr), 
            bindShaderStage(ShaderType::InvalidShaderType),
            bindSlotIndex(InvalidIndex) {};

        ID3D11Buffer* constantBuffer;
        ShaderType::Code bindShaderStage;
        int32 bindSlotIndex;
    };
    uint32 selMask;
    int32 selIndex;
    int32 numPrograms;
    StaticArray<programEntry, ProgramBundleSetup::MaxNumPrograms> programEntries;
    int32 numConstantBuffers;
    StaticArray<cbEntry, ProgramBundleSetup::MaxNumUniformBlocks> cbEntries;
};

//------------------------------------------------------------------------------
inline uint32
d3d11ProgramBundle::getSelectionMask() const {
    return this->selMask;
}

//------------------------------------------------------------------------------
inline uint32
d3d11ProgramBundle::getSelectionIndex() const {
    return this->selIndex;
}

//------------------------------------------------------------------------------
inline bool
d3d11ProgramBundle::select(uint32 mask) {
    // number of programs will be small, so linear is ok
    if (this->selMask != mask) {
        for (int32 i = 0; i < this->numPrograms; i++) {
            if (this->programEntries[i].mask == mask) {
                this->selMask = mask;
                this->selIndex = i;
                return true;
            }
        }
    }
    return false;
}

//------------------------------------------------------------------------------
inline ID3D11VertexShader*
d3d11ProgramBundle::getSelectedVertexShader() const {
    return this->programEntries[this->selIndex].vertexShader;
}

//------------------------------------------------------------------------------
inline ID3D11PixelShader*
d3d11ProgramBundle::getSelectedPixelShader() const {
    return this->programEntries[this->selIndex].pixelShader;
}

//------------------------------------------------------------------------------
inline int32
d3d11ProgramBundle::getNumConstantBuffers() const {
    return this->numConstantBuffers;
}

//------------------------------------------------------------------------------
inline ID3D11Buffer*
d3d11ProgramBundle::getConstantBufferAt(int32 index, ShaderType::Code& outBindShaderStage, int32& outBindSlotIndex) const {
    const cbEntry& entry = this->cbEntries[index];
    outBindShaderStage = entry.bindShaderStage;
    outBindSlotIndex = entry.bindSlotIndex;
    return entry.constantBuffer;
}

} // namespace _priv
} // namespace Oryol
