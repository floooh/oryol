//------------------------------------------------------------------------------
//  d3d11Shader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11Shader.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d11Shader::d3d11Shader() :
d3d11VertexShader(nullptr),
d3d11PixelShader(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
d3d11Shader::~d3d11Shader() {
    o_assert_dbg(nullptr == this->d3d11VertexShader);
    o_assert_dbg(nullptr == this->d3d11PixelShader);
}

//------------------------------------------------------------------------------
void
d3d11Shader::Clear() {
    this->d3d11VertexShader = nullptr;
    this->d3d11PixelShader = nullptr;
    shaderBase::Clear();
}

} // namespace _priv
} // namespace Oryol
