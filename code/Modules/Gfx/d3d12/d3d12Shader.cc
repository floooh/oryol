//------------------------------------------------------------------------------
//  d3d12Shader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12Shader.h"
#include "d3d12_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d12Shader::d3d12Shader() {
    this->Clear();
}

//------------------------------------------------------------------------------
d3d12Shader::~d3d12Shader() {
#if ORYOL_DEBUG
    for (int32 i = 0; i < this->numPrograms; i++) {
        o_assert_dbg(nullptr == this->programEntries[i].vertexShader.ptr);
        o_assert_dbg(nullptr == this->programEntries[i].pixelShader.ptr);
    }
#endif
}

//------------------------------------------------------------------------------
void
d3d12Shader::Clear() {
    this->selMask = 0xFFFFFFFF;
    this->selIndex = 0;
    this->numPrograms = 0;
    this->programEntries.Fill(programEntry());
    shaderBase::Clear();
}

//------------------------------------------------------------------------------
int32
d3d12Shader::addShaders(uint32 mask, const shaderBlob& vs, const shaderBlob& ps) {
    o_assert_dbg(this->numPrograms < GfxConfig::MaxNumBundlePrograms);
    o_assert_dbg(vs.ptr && (vs.size > 0) && ps.ptr && (ps.size > 0));

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
d3d12Shader::getNumPrograms() const {
    return this->numPrograms;
}

//------------------------------------------------------------------------------
const d3d12Shader::shaderBlob&
d3d12Shader::getVertexShaderAt(int32 index) const {
    return this->programEntries[index].vertexShader;
}

//------------------------------------------------------------------------------
const d3d12Shader::shaderBlob&
d3d12Shader::getPixelShaderAt(int32 index) const {
    return this->programEntries[index].pixelShader;
}

} // namespace _priv
} // namespace Oryol
