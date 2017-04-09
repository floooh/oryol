//------------------------------------------------------------------------------
//  d3d11ShaderFactory
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Gfx/Core/renderer.h"
#include "d3d11ShaderFactory.h"
#include "Gfx/Resource/resource.h"
#include "d3d11_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d11ShaderFactory::~d3d11ShaderFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
d3d11ShaderFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptrs;
    this->d3d11Device = this->pointers.renderer->d3d11Device;
}

//------------------------------------------------------------------------------
void 
d3d11ShaderFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    this->pointers = gfxPointers();
    this->d3d11Device = nullptr;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11ShaderFactory::SetupResource(shader& shd) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(this->d3d11Device);
    o_assert_dbg(nullptr == shd.d3d11VertexShader);
    o_assert_dbg(nullptr == shd.d3d11PixelShader);
    HRESULT hr;

    this->pointers.renderer->invalidateShaderState();
    const ShaderLang::Code slang = ShaderLang::HLSL5;
    const ShaderSetup& setup = shd.Setup;

    // create vertex shader
    const void* vsPtr = nullptr;
    uint32_t vsSize = 0;
    setup.VertexShaderByteCode(slang, vsPtr, vsSize);
    o_assert_dbg(vsPtr);
    hr = this->d3d11Device->CreateVertexShader(vsPtr, vsSize, NULL, &shd.d3d11VertexShader);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(shd.d3d11VertexShader);

    // create pixel shader
    const void* psPtr = nullptr;
    uint32_t psSize = 0;
    setup.FragmentShaderByteCode(slang, psPtr, psSize);
    o_assert_dbg(psPtr);
    hr = this->d3d11Device->CreatePixelShader(psPtr, psSize, NULL, &shd.d3d11PixelShader);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(shd.d3d11PixelShader);

    // create constant buffers
    D3D11_BUFFER_DESC cbDesc = { };
    for (int i = 0; i < setup.NumUniformBlocks(); i++) {
        const ShaderStage::Code bindStage = setup.UniformBlockBindStage(i);
        const int bindSlot = setup.UniformBlockBindSlot(i);
        const UniformBlockLayout& layout = setup.UniformBlockLayout(i);
        o_assert_dbg(InvalidIndex != bindSlot);

        // NOTE: constant buffer size must be multiple of 16 bytes
        o_assert_dbg(layout.ByteSize() > 0);
        cbDesc.ByteWidth = Memory::RoundUp(layout.ByteSize(), 16);
        cbDesc.Usage = D3D11_USAGE_DEFAULT;
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.CPUAccessFlags = 0;

        ID3D11Buffer* d3d11ConstantBuffer = nullptr;
        hr = this->d3d11Device->CreateBuffer(&cbDesc, nullptr, &d3d11ConstantBuffer);
        o_assert(SUCCEEDED(hr));
        o_assert_dbg(d3d11ConstantBuffer);

        // the d3d11ConstantBuffer ptr can be 0 at this point, if the
        // uniform block only contains textures
        shd.addUniformBlockEntry(bindStage, bindSlot, d3d11ConstantBuffer);
    }

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
d3d11ShaderFactory::DestroyResource(shader& shd) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(this->d3d11Device);

    this->pointers.renderer->invalidateShaderState();
    if (shd.d3d11VertexShader) {
        shd.d3d11VertexShader->Release();
    }
    if (shd.d3d11PixelShader) {
        shd.d3d11PixelShader->Release();
    }
    for (int bindStage = 0; bindStage < ShaderStage::NumShaderStages; bindStage++) {
        for (int bindSlot = 0; bindSlot < GfxConfig::MaxNumUniformBlocksPerStage; bindSlot++) {
            ID3D11Buffer* cb = shd.getConstantBuffer((ShaderStage::Code)bindStage, bindSlot);
            if (cb) {
                cb->Release();
            }
        }
    }
    shd.Clear();
}

} // namespace _priv
} // namespace Oryol
