//------------------------------------------------------------------------------
//  mtlShader.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlShader.h"
#include "mtl_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mtlShader::mtlShader() {
    this->Clear();
}

//------------------------------------------------------------------------------
mtlShader::~mtlShader() {
#if ORYOL_DEBUG
    o_assert_dbg(nil == this->mtlLibrary);
    o_assert_dbg(nil == this->mtlVertexShader);
    o_assert_dbg(nil == this->mtlFragmentShader);
#endif
}

//------------------------------------------------------------------------------
void
mtlShader::Clear() {
    this->mtlLibrary = nil;
    this->mtlVertexShader = nil;
    this->mtlFragmentShader = nil;
    shaderBase::Clear();
}

} // namespace _priv
} // namespace Oryol
