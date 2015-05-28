//------------------------------------------------------------------------------
//  d3d11ShaderFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11ShaderFactory.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Resource/shader.h"
#include "Gfx/d3d11/d3d11_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d11ShaderFactory::d3d11ShaderFactory() :
d3d11Device(nullptr),
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
d3d11ShaderFactory::~d3d11ShaderFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
d3d11ShaderFactory::Setup(class renderer* rendr) {
    o_assert_dbg(!this->isValid);
    o_assert_dbg(rendr);
    o_assert_dbg(rendr->d3d11Device);

    this->isValid = true;
    this->d3d11Device = rendr->d3d11Device;
}

//------------------------------------------------------------------------------
void
d3d11ShaderFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->d3d11Device = nullptr;
    this->isValid = false;
}

//------------------------------------------------------------------------------
bool
d3d11ShaderFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11ShaderFactory::SetupResource(shader& shd) {
    o_assert_dbg(this->d3d11Device);

    const ShaderSetup& setup = shd.Setup;

    // compile vertex or fragment shader
    HRESULT hr;
    const void* ptr = nullptr;
    uint32 size = 0;
    setup.ByteCode(ShaderLang::HLSL5, ptr, size);
    o_assert_dbg(ptr && (size > 0));
    if (setup.Type == ShaderType::VertexShader) {
        hr = this->d3d11Device->CreateVertexShader(ptr, size, NULL, &shd.d3d11VertexShader);
        o_assert(SUCCEEDED(hr));
        o_assert_dbg(shd.d3d11VertexShader);
    }
    else {
        o_assert_dbg(setup.Type == ShaderType::FragmentShader);
        hr = this->d3d11Device->CreatePixelShader(ptr, size, NULL, &shd.d3d11PixelShader);
        o_assert(SUCCEEDED(hr));
        o_assert_dbg(shd.d3d11PixelShader);
    }

    // initialize the rest of the shader object
    shd.shaderType = setup.Type;

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
d3d11ShaderFactory::DestroyResource(shader& shd) {
    o_assert_dbg(this->isValid);

    if (shd.d3d11VertexShader) {
        shd.d3d11VertexShader->Release();
        shd.d3d11VertexShader = nullptr;
    }
    if (shd.d3d11PixelShader) {
        shd.d3d11PixelShader->Release();
        shd.d3d11PixelShader = nullptr;
    }
}

} // namespace _priv
} // namespace Oryol
