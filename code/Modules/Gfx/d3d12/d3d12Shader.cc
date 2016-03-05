//------------------------------------------------------------------------------
//  d3d12Shader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12Shader.h"
#include "d3d12_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d12Shader::~d3d12Shader() {
    o_assert_dbg(nullptr == this->vertexShader.ptr);
    o_assert_dbg(nullptr == this->pixelShader.ptr);
}

//------------------------------------------------------------------------------
void
d3d12Shader::Clear() {
    this->vertexShader = shaderBlob();
    this->pixelShader = shaderBlob();
    shaderBase::Clear();
}

} // namespace _priv
} // namespace Oryol
