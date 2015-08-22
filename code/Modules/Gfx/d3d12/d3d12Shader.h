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
    /// select a vs/ps pair
    bool select(uint32 mask);
    /// get the current selection mask
    uint32 getSelectionMask() const;
    /// get the current selection index
    int32 getSelectionIndex() const;
    /// get the currently selected vertex shader
    const shaderBlob& getSelectedVertexShader() const;
    /// get the currently selected pixel shader
    const shaderBlob& getSelectedPixelShader() const;

    /// get number of programs
    int32 getNumPrograms() const;
    /// get vertex shader at program index
    const shaderBlob& getVertexShaderAt(int32 index) const;
    /// get pixel shader at program index
    const shaderBlob& getPixelShaderAt(int32 index) const;


private:
    struct programEntry {
        programEntry() : mask(0) { };
        uint32 mask;
        shaderBlob vertexShader;
        shaderBlob pixelShader;
    };
    uint32 selMask;
    int32 selIndex;
    int32 numPrograms;
    StaticArray<programEntry, GfxConfig::MaxNumBundlePrograms> programEntries;
};

//------------------------------------------------------------------------------
inline uint32
d3d12Shader::getSelectionMask() const {
    return this->selMask;
}

//------------------------------------------------------------------------------
inline int32
d3d12Shader::getSelectionIndex() const {
    return this->selIndex;
}

//------------------------------------------------------------------------------
inline bool
d3d12Shader::select(uint32 mask) {
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
inline const d3d12Shader::shaderBlob&
d3d12Shader::getSelectedVertexShader() const {
    return this->programEntries[this->selIndex].vertexShader;
}

//------------------------------------------------------------------------------
inline const d3d12Shader::shaderBlob&
d3d12Shader::getSelectedPixelShader() const {
    return this->programEntries[this->selIndex].pixelShader;
}

} // namespace _priv
} // namespace Oryol
