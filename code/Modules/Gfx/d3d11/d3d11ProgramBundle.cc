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
    for (int32 i = 0; i < this->numProgramEntries; i++) {
        o_assert_dbg(0 == this->programEntries[i].vertexShader);
        o_assert_dbg(0 == this->programEntries[i].pixelShader);
    }
#endif
}

//------------------------------------------------------------------------------
void
d3d11ProgramBundle::Clear() {
    this->selMask = 0xFFFFFFFF;
    this->selIndex = 0;
    this->numProgramEntries = 0;
    for (auto& entry : this->programEntries) {
        entry.mask = 0;
        entry.vertexShader = nullptr;
        entry.pixelShader = nullptr;
    }
    programBundleBase::Clear();
}

//------------------------------------------------------------------------------
int32
d3d11ProgramBundle::add(uint32 mask, ID3D11VertexShader* vs, ID3D11PixelShader* ps) {
    o_assert_dbg(this->numProgramEntries < MaxNumPrograms);
    o_assert_dbg(vs && ps);

    // make sure the mask is unique
    for (int32 i = 0; i < this->numProgramEntries; i++) {
        o_assert_dbg(this->programEntries[i].mask != mask);
    }

    auto& entry = this->programEntries[this->numProgramEntries];
    entry.mask = mask;
    entry.vertexShader = vs;
    entry.pixelShader = ps;
    return this->numProgramEntries++;
}

//------------------------------------------------------------------------------
int32
d3d11ProgramBundle::getNumPrograms() const {
    return this->numProgramEntries;
}

//------------------------------------------------------------------------------
ID3D11VertexShader*
d3d11ProgramBundle::getVertexShaderAtIndex(int32 index) const {
    o_assert_range_dbg(index, this->numProgramEntries);
    return this->programEntries[index].vertexShader;
}

//------------------------------------------------------------------------------
ID3D11PixelShader*
d3d11ProgramBundle::getPixelShaderAtIndex(int32 index) const {
    o_assert_range_dbg(index, this->numProgramEntries);
    return this->programEntries[index].pixelShader;
}

} // namespace _priv
} // namespace Oryol