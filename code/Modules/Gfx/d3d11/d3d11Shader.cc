//------------------------------------------------------------------------------
//  d3d11Shader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11Shader.h"
#include "d3d11_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d11Shader::d3d11Shader() {
    this->Clear();
}

//------------------------------------------------------------------------------
d3d11Shader::~d3d11Shader() {
#if ORYOL_DEBUG
    for (int32 i = 0; i < this->numPrograms; i++) {
        o_assert_dbg(nullptr == this->programEntries[i].vertexShader);
        o_assert_dbg(nullptr == this->programEntries[i].pixelShader);
    }
    for (auto cb : this->constantBuffers) {
        o_assert_dbg(nullptr == cb);
    }
#endif
}

//------------------------------------------------------------------------------
void
d3d11Shader::Clear() {
    this->numPrograms = 0;
    this->programEntries.Fill(programEntry());
    this->constantBuffers.Fill(nullptr);
    shaderBase::Clear();
}

//------------------------------------------------------------------------------
int32
d3d11Shader::addShaders(uint32 mask, ID3D11VertexShader* vs, ID3D11PixelShader* ps) {
    o_assert_dbg(this->numPrograms < GfxConfig::MaxNumBundlePrograms);
    o_assert_dbg(vs && ps);

    // make sure the mask is unique
    for (int32 i = 0; i < this->numPrograms; i++) {
        o_assert_dbg(this->programEntries[i].mask != mask);
    }

    auto& entry = this->programEntries[this->numPrograms];
    entry.mask = mask;
    entry.vertexShader = vs;
    entry.pixelShader = ps;
    return this->numPrograms++;
}

//------------------------------------------------------------------------------
int32
d3d11Shader::getNumPrograms() const {
    return this->numPrograms;
}

//------------------------------------------------------------------------------
ID3D11VertexShader*
d3d11Shader::getVertexShaderAtIndex(int32 index) const {
    return this->programEntries[index].vertexShader;
}

//------------------------------------------------------------------------------
ID3D11PixelShader*
d3d11Shader::getPixelShaderAtIndex(int32 index) const {
    return this->programEntries[index].pixelShader;
}

//------------------------------------------------------------------------------
int32
d3d11Shader::getProgIndexByMask(uint32 mask) const {
    for (int i = 0; i < this->numPrograms; i++) {
        if (this->programEntries[i].mask == mask) {
            return i;
        }
    }
    return InvalidIndex;
}

//------------------------------------------------------------------------------
ID3D11VertexShader*
d3d11Shader::getVertexShaderByMask(uint32 mask) const {
    int32 progIndex = this->getProgIndexByMask(mask);
    if (progIndex != InvalidIndex) {
        return this->programEntries[progIndex].vertexShader;
    }
    else {
        return nullptr;
    }
}

//------------------------------------------------------------------------------
ID3D11PixelShader*
d3d11Shader::getPixelShaderByMask(uint32 mask) const {
    int32 progIndex = this->getProgIndexByMask(mask);
    if (progIndex != InvalidIndex) {
        return this->programEntries[progIndex].pixelShader;
    }
    else {
        return nullptr;
    }
}

//------------------------------------------------------------------------------
void
d3d11Shader::addUniformBlockEntry(ShaderStage::Code bindStage, int32 bindSlot, ID3D11Buffer* cb) {
    o_assert_dbg(cb);
    const int32 cbIndex = (GfxConfig::MaxNumUniformBlocksPerStage * bindStage) + bindSlot;
    this->constantBuffers[cbIndex] = cb;
}

} // namespace _priv
} // namespace Oryol
