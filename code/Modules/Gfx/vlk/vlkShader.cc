//------------------------------------------------------------------------------
//  vlkShader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "vlkShader.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
vlkShader::~vlkShader() {
    o_assert_dbg(!this->vsModule);
    o_assert_dbg(!this->fsModule);
}

//------------------------------------------------------------------------------
void
vlkShader::Clear() {
    this->vsModule = nullptr;
    this->fsModule = nullptr;
    shaderBase::Clear();
}

} // namespace _priv
} // namespace Oryol
