#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11ProgramBundle
    @ingroup _priv
    @brief D3D11 implementation of program bundle
*/
#include "Gfx/Resource/programBundleBase.h"
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
    int32 add(uint32 mask, ID3D11VertexShader* vs, ID3D11PixelShader* ps);
    /// select a vs/ps pair
    bool select(uint32 mask);
    /// get the current selection mask
    uint32 getSelectionMask() const;
    /// get the currently selected vertex shader
    ID3D11VertexShader* getVertexShader() const;
    /// get the currently selected pixel shader
    ID3D11PixelShader* getPixelShader() const;

    /// get number of programs
    int32 getNumPrograms() const;
    /// get vertex shader at program index
    ID3D11VertexShader* getVertexShaderAtIndex(int32 index) const;
    /// get pixel shader at program index
    ID3D11PixelShader* getPixelShaderAtIndex(int32 index) const;

private:
    static const int32 MaxNumPrograms = 8;

    struct programEntry {
        uint32 mask;
        ID3D11VertexShader* vertexShader;
        ID3D11PixelShader* pixelShader;
    };
    uint32 selMask;
    int32 selIndex;
    int32 numProgramEntries;
    programEntry programEntries[MaxNumPrograms];
};

//------------------------------------------------------------------------------
inline uint32
d3d11ProgramBundle::getSelectionMask() const {
    return this->selMask;
}

//------------------------------------------------------------------------------
inline bool
d3d11ProgramBundle::select(uint32 mask) {
    // number of programs will be small, so linear is ok
    if (this->selMask != mask) {
        for (int32 i = 0; i < this->numProgramEntries; i++) {
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
d3d11ProgramBundle::getVertexShader() const {
    return this->programEntries[this->selIndex].vertexShader;
}

//------------------------------------------------------------------------------
inline ID3D11PixelShader*
d3d11ProgramBundle::getPixelShader() const {
    return this->programEntries[this->selIndex].pixelShader;
}

} // namespace _priv
} // namespace Oryol
