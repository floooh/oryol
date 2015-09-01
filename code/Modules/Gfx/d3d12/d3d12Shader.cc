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
    this->numPrograms = 0;
    this->programEntries.Fill(programEntry());
    this->numUniformBlockEntries = 0;
    this->uniformBlockEntries.Fill(ubEntry());
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
const d3d12Shader::shaderBlob*
d3d12Shader::getVertexShaderByMask(uint32 mask) const {
    for (int i = 0; i < this->numPrograms; i++) {
        if (this->programEntries[i].mask == mask) {
            return &this->programEntries[i].vertexShader;
        }
    }
    return nullptr;
}

//------------------------------------------------------------------------------
const d3d12Shader::shaderBlob*
d3d12Shader::getPixelShaderByMask(uint32 mask) const {
    for (int i = 0; i < this->numPrograms; i++) {
        if (this->programEntries[i].mask == mask) {
            return &this->programEntries[i].pixelShader;
        }
    }
    return nullptr;
}

//------------------------------------------------------------------------------
void
d3d12Shader::addUniformBlockEntry(ShaderType::Code bindShaderStage, int32 bindSlotIndex) {
    ubEntry& entry = this->uniformBlockEntries[this->numUniformBlockEntries++];
    entry.bindShaderStage = bindShaderStage;
    entry.bindSlotIndex = bindSlotIndex;
}

//------------------------------------------------------------------------------
int32
d3d12Shader::getNumUniformBlocks() const {
    return this->numUniformBlockEntries;
}

//------------------------------------------------------------------------------
ShaderType::Code
d3d12Shader::getUniformBlockShaderStage(int32 ubIndex) const {
    return this->uniformBlockEntries[ubIndex].bindShaderStage;
}

//------------------------------------------------------------------------------
int32
d3d12Shader::getUniformBlockBindSlotIndex(int32 ubIndex) const {
    return this->uniformBlockEntries[ubIndex].bindSlotIndex;
}

} // namespace _priv
} // namespace Oryol
