//------------------------------------------------------------------------------
//  d3d12ShaderFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Assertion.h"
#include "d3d12ShaderFactory.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Resource/shader.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d12ShaderFactory::~d3d12ShaderFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
d3d12ShaderFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptrs;
}

//------------------------------------------------------------------------------
void
d3d12ShaderFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    this->pointers = gfxPointers();
}

//------------------------------------------------------------------------------
bool
d3d12ShaderFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d12ShaderFactory::SetupResource(shader& shd) {
    o_assert_dbg(this->isValid);

    const ShaderLang::Code slang = ShaderLang::HLSL5;
    const ShaderSetup& setup = shd.Setup;
    setup.VertexShaderByteCode(slang, shd.vertexShader.ptr, shd.vertexShader.size);
    setup.FragmentShaderByteCode(slang, shd.pixelShader.ptr, shd.pixelShader.size);
    o_assert_dbg(shd.vertexShader.ptr && (shd.vertexShader.size > 0));
    o_assert_dbg(shd.pixelShader.ptr && (shd.pixelShader.size > 0));

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
d3d12ShaderFactory::DestroyResource(shader& shd) {
    o_assert_dbg(this->isValid);
    shd.Clear();
}

} // namespace _priv
} // namespace Oryol
