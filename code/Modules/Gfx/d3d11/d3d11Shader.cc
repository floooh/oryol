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
    o_assert_dbg(nullptr == this->d3d11VertexShader);
    o_assert_dbg(nullptr == this->d3d11PixelShader);
    for (auto cb : this->constantBuffers) {
        o_assert_dbg(nullptr == cb);
    }
#endif
}

//------------------------------------------------------------------------------
void
d3d11Shader::Clear() {
    this->d3d11VertexShader = nullptr;
    this->d3d11PixelShader = nullptr;
    this->constantBuffers.Fill(nullptr);
    shaderBase::Clear();
}

//------------------------------------------------------------------------------
void
d3d11Shader::addUniformBlockEntry(ShaderStage::Code bindStage, int bindSlot, ID3D11Buffer* cb) {
    o_assert_dbg(cb);
    const int cbIndex = (GfxConfig::MaxNumUniformBlocksPerStage * bindStage) + bindSlot;
    this->constantBuffers[cbIndex] = cb;
}

} // namespace _priv
} // namespace Oryol
