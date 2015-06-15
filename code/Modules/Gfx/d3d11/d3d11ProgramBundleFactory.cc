//------------------------------------------------------------------------------
//  d3d11ProgramBundleFactory
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Gfx/Core/renderer.h"
#include "d3d11ProgramBundleFactory.h"
#include "d3d11_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d11ProgramBundleFactory::d3d11ProgramBundleFactory() :
renderer(nullptr),
d3d11Device(nullptr),
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
d3d11ProgramBundleFactory::~d3d11ProgramBundleFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
d3d11ProgramBundleFactory::Setup(class renderer* rendr, shaderPool* shdPool, shaderFactory* shdFactory) {
    o_assert_dbg(!this->isValid);
    o_assert_dbg(rendr);
    o_assert_dbg(rendr->d3d11Device);
    this->isValid = true;
    this->renderer = rendr;
    this->d3d11Device = rendr->d3d11Device;
}

//------------------------------------------------------------------------------
void 
d3d11ProgramBundleFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    this->d3d11Device = nullptr;
}

//------------------------------------------------------------------------------
bool
d3d11ProgramBundleFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11ProgramBundleFactory::SetupResource(programBundle& progBundle) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(this->d3d11Device);
    HRESULT hr;

    this->renderer->invalidateProgramState();
    const ShaderLang::Code slang = ShaderLang::HLSL5;
    const ProgramBundleSetup& setup = progBundle.Setup;

    // for each program in the bundle
    const int32 numProgs = setup.NumPrograms();
    for (int32 progIndex = 0; progIndex < numProgs; progIndex++) {

        // create vertex shader (only support byte code)
        ID3D11VertexShader* vs = nullptr;
        const void* vsPtr = nullptr;
        uint32 vsSize = 0;
        setup.VertexShaderByteCode(progIndex, slang, vsPtr, vsSize);
        o_assert_dbg(vsPtr);
        hr = this->d3d11Device->CreateVertexShader(vsPtr, vsSize, NULL, &vs);
        o_assert(SUCCEEDED(hr));
        o_assert_dbg(vs);

        // create pixel shader
        ID3D11PixelShader* ps = nullptr;
        const void* psPtr = nullptr;
        uint32 psSize = 0;
        setup.FragmentShaderByteCode(progIndex, slang, psPtr, psSize);
        o_assert_dbg(psPtr);
        hr = this->d3d11Device->CreatePixelShader(psPtr, psSize, NULL, &ps);
        o_assert(SUCCEEDED(hr));
        o_assert_dbg(ps);

        // add vertexshader/pixelshader pair to program bundle
        progBundle.addShaders(setup.Mask(progIndex), vs, ps);
    }

    // create constant buffers
    D3D11_BUFFER_DESC cbDesc;
    Memory::Clear(&cbDesc, sizeof(cbDesc));
    for (int i = 0; i < setup.NumUniformBlocks(); i++) {
        const int32 bindSlotIndex = setup.UniformBlockSlot(i);
        const UniformLayout& layout = setup.UniformBlockLayout(i);
        const ShaderType::Code bindShaderStage = setup.UniformBlockShaderStage(i);

        ID3D11Buffer* d3d11ConstantBuffer = nullptr;
        if (bindSlotIndex != InvalidIndex) {

            // NOTE: constant buffer size must be multiple of 16 bytes
            o_assert_dbg(layout.ByteSizeWithoutTextures() > 0);
            cbDesc.ByteWidth = Memory::RoundUp(layout.ByteSizeWithoutTextures(), 16);
            cbDesc.Usage = D3D11_USAGE_DYNAMIC;
            cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            hr = this->d3d11Device->CreateBuffer(&cbDesc, nullptr, &d3d11ConstantBuffer);
            o_assert(SUCCEEDED(hr));
            o_assert_dbg(d3d11ConstantBuffer);
        }

        // the d3d11ConstantBuffer ptr can be 0 at this point, if the
        // uniform block only contains textures
        progBundle.addUniformBlockEntry(d3d11ConstantBuffer, bindShaderStage, bindSlotIndex);
    }
    o_assert_dbg(progBundle.getNumUniformBlockEntries() == setup.NumUniformBlocks());

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
d3d11ProgramBundleFactory::DestroyResource(programBundle& progBundle) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(this->d3d11Device);

    const int32 numProgs = progBundle.getNumPrograms();
    for (int32 progIndex = 0; progIndex < numProgs; progIndex++) {
        ID3D11VertexShader* vs = progBundle.getVertexShaderAt(progIndex);
        if (vs) {
            vs->Release();
        }
        ID3D11PixelShader* ps = progBundle.getPixelShaderAt(progIndex);
        if (ps) {
            ps->Release();
        }
    }

    int32 dummySlotIndex = 0;
    ShaderType::Code dummyBindStage = ShaderType::InvalidShaderType;
    const int32 numConstantBuffers = progBundle.getNumUniformBlockEntries();
    for (int32 cbIndex = 0; cbIndex < numConstantBuffers; cbIndex++) {
        ID3D11Buffer* cb = progBundle.getUniformBlockEntryAt(cbIndex, dummyBindStage, dummySlotIndex);
        if (cb) {
            cb->Release();
        }
    }

    progBundle.Clear();
}

} // namespace _priv
} // namespace Oryol
