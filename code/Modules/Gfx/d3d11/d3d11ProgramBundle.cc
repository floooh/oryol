//------------------------------------------------------------------------------
//  d3d11ProgramBundle.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11ProgramBundle.h"
#include "d3d11_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d11ProgramBundle::d3d11ProgramBundle() {
    this->Clear();
}

//------------------------------------------------------------------------------
d3d11ProgramBundle::~d3d11ProgramBundle() {
#if ORYOL_DEBUG
    for (int32 i = 0; i < this->numPrograms; i++) {
        o_assert_dbg(nullptr == this->programEntries[i].vertexShader);
        o_assert_dbg(nullptr == this->programEntries[i].pixelShader);
    }
    for (int32 i = 0; i < this->numConstantBuffers; i++) {
        o_assert_dbg(nullptr == this->cbEntries[i].constantBuffer);
    }
#endif
}

//------------------------------------------------------------------------------
void
d3d11ProgramBundle::Clear() {
    this->selMask = 0xFFFFFFFF;
    this->selIndex = 0;
    this->numPrograms = 0;
    this->programEntries.Fill(programEntry());
    this->numConstantBuffers = 0;
    this->cbEntries.Fill(cbEntry());
    programBundleBase::Clear();
}

//------------------------------------------------------------------------------
int32
d3d11ProgramBundle::addShaders(uint32 mask, ID3D11VertexShader* vs, ID3D11PixelShader* ps) {
    o_assert_dbg(this->numPrograms < ProgramBundleSetup::MaxNumPrograms);
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
d3d11ProgramBundle::getNumPrograms() const {
    return this->numPrograms;
}

//------------------------------------------------------------------------------
ID3D11VertexShader*
d3d11ProgramBundle::getVertexShaderAt(int32 index) const {
    return this->programEntries[index].vertexShader;
}

//------------------------------------------------------------------------------
ID3D11PixelShader*
d3d11ProgramBundle::getPixelShaderAt(int32 index) const {
    return this->programEntries[index].pixelShader;
}

//------------------------------------------------------------------------------
int32
d3d11ProgramBundle::addConstantBuffer(ID3D11Buffer* cb, ShaderType::Code bindShaderStage, int32 bindSlotIndex) {
    o_assert_dbg(this->numConstantBuffers < ProgramBundleSetup::MaxNumUniformBlocks);
    o_assert_dbg(cb);
    cbEntry& entry = this->cbEntries[this->numConstantBuffers];
    entry.constantBuffer = cb;
    entry.bindShaderStage = bindShaderStage;
    entry.bindSlotIndex = bindSlotIndex;
    return this->numConstantBuffers++;
}

} // namespace _priv
} // namespace Oryol