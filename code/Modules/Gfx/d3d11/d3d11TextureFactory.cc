//------------------------------------------------------------------------------
//  d3d11TextureFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11TextureFactory.h"
#include "Gfx/d3d11/d3d11_impl.h"
#include "Gfx/d3d11/d3d11types.h"
#include "Gfx/Core/renderer.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d11TextureFactory::d3d11TextureFactory() :
renderer(nullptr),
displayManager(nullptr),
texPool(nullptr),
d3d11Device(nullptr),
isValid(false) {
    // empty
}
    
//------------------------------------------------------------------------------
d3d11TextureFactory::~d3d11TextureFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
d3d11TextureFactory::Setup(class renderer* rendr, displayMgr* displayMgr, texturePool* texPool_) {
    o_assert_dbg(!this->isValid);
    o_assert_dbg(nullptr != rendr);
    o_assert_dbg(nullptr != displayMgr);
    o_assert_dbg(nullptr != texPool_);

    this->isValid = true;
    this->renderer = rendr;
    this->displayManager = displayMgr;
    this->texPool = texPool_;
    this->d3d11Device = renderer->d3d11Device;
}

//------------------------------------------------------------------------------
void
d3d11TextureFactory::Discard() {
    o_assert_dbg(this->isValid);

    this->isValid = false;
    this->d3d11Device = nullptr;
    this->renderer = nullptr;
    this->displayManager = nullptr;
    this->texPool = nullptr;
}

//------------------------------------------------------------------------------
bool
d3d11TextureFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11TextureFactory::SetupResource(texture& tex) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(!tex.Setup.ShouldSetupFromPixelData());
    o_assert_dbg(!tex.Setup.ShouldSetupFromFile());

    if (tex.Setup.ShouldSetupAsRenderTarget()) {
        return this->createRenderTarget(tex);
    }
    else {
        // here would go more ways to create textures without image data
        return ResourceState::InvalidState;
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11TextureFactory::SetupResource(texture& tex, const void* data, int32 size) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(!tex.Setup.ShouldSetupAsRenderTarget());
    o_assert_dbg(!tex.Setup.ShouldSetupFromFile());

    if (tex.Setup.ShouldSetupFromPixelData()) {
        return this->createFromPixelData(tex, data, size);
    }
    else {
        // here would go more ways to create textures with image data
        return ResourceState::InvalidState;
    }
}

//------------------------------------------------------------------------------
void
d3d11TextureFactory::DestroyResource(texture& tex) {
    o_assert_dbg(this->isValid);

    this->renderer->invalidateTextureState();

    if (nullptr != tex.d3d11Texture2D) {
        tex.d3d11Texture2D->Release();
    }
    if (nullptr != tex.d3d11ShaderResourceView) {
        tex.d3d11ShaderResourceView->Release();
    }
    if (nullptr != tex.d3d11SamplerState) {
        tex.d3d11SamplerState->Release();
    }
    if (nullptr != tex.d3d11RenderTargetView) {
        tex.d3d11RenderTargetView->Release();
    }
    if (nullptr != tex.d3d11DepthBufferTexture) {
        tex.d3d11DepthBufferTexture->Release();
    }
    if (nullptr != tex.d3d11DepthStencilView) {
        tex.d3d11DepthStencilView->Release();
    }
    tex.Clear();
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11TextureFactory::createRenderTarget(texture& tex) {
    // FIXME
    return ResourceState::InvalidState;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11TextureFactory::createFromPixelData(texture& tex, const void* data, int32 size) {
    o_assert_dbg(nullptr != this->d3d11Device);
    o_assert_dbg(nullptr == tex.d3d11Texture2D);
    o_assert_dbg(nullptr == tex.d3d11ShaderResourceView);
    o_assert_dbg(nullptr == tex.d3d11SamplerState);
    o_assert_dbg(nullptr != data);
    o_assert_dbg(size > 0);

    const TextureSetup& setup = tex.Setup;
    o_assert_dbg(setup.NumMipMaps > 0);

    // FIXME: test if texture format is supported

    if (setup.Type == TextureType::Texture3D) {
        o_warn("d3d11TextureFactory: 3d textures not yet implemented\n");
        return ResourceState::Failed;
    }

    // create the 2D or Cube texture object with data
    D3D11_TEXTURE2D_DESC texDesc;
    Memory::Clear(&texDesc, sizeof(texDesc));
    texDesc.Width = setup.Width;
    texDesc.Height = setup.Height;
    texDesc.MipLevels = setup.NumMipMaps;
    texDesc.ArraySize = setup.Type == TextureType::TextureCube ? 6 : 1;
    texDesc.Format = d3d11Types::asTextureFormat(setup.ColorFormat);
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_IMMUTABLE;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = setup.Type == TextureType::TextureCube ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;

    const int32 maxNumSubResourceData = TextureSetup::MaxNumFaces * TextureSetup::MaxNumMipMaps;
    D3D11_SUBRESOURCE_DATA subResourceData[maxNumSubResourceData] = { 0 };
    const uint8* srcPtr = (const uint8*) data;
    const int numFaces = setup.Type == TextureType::TextureCube ? 6 : 1;
    const int numMipMaps = setup.NumMipMaps;
    int subResourceDataIndex = 0;
    for (int faceIndex = 0; faceIndex < numFaces; faceIndex++) {
        for (int mipIndex = 0; mipIndex < numMipMaps; mipIndex++, subResourceDataIndex++) {
            o_assert_dbg(subResourceDataIndex < maxNumSubResourceData);
            D3D11_SUBRESOURCE_DATA& subResData = subResourceData[subResourceDataIndex];
            subResData.pSysMem = srcPtr + setup.ImageOffsets[faceIndex][mipIndex];
            subResData.SysMemPitch = PixelFormat::RowPitch(setup.ColorFormat, setup.Width >> mipIndex);
            subResData.SysMemSlicePitch = 0;
        }
    }
    HRESULT hr = this->d3d11Device->CreateTexture2D(&texDesc, subResourceData, &tex.d3d11Texture2D);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(nullptr != tex.d3d11Texture2D);

    // create shader-resource-view object
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    Memory::Clear(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = texDesc.Format;
    if (setup.Type == TextureType::TextureCube) {
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
        srvDesc.TextureCube.MostDetailedMip = 0;
        srvDesc.TextureCube.MipLevels = setup.NumMipMaps;
    }
    else {
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = setup.NumMipMaps;
    }
    hr = this->d3d11Device->CreateShaderResourceView(tex.d3d11Texture2D, &srvDesc, &tex.d3d11ShaderResourceView);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(nullptr != tex.d3d11ShaderResourceView);

    // create sampler object (note: samplers with identical state are automatically shared by D3D11)
    D3D11_SAMPLER_DESC smpDesc;
    Memory::Clear(&smpDesc, sizeof(smpDesc));
    smpDesc.Filter = d3d11Types::asSamplerFilter(setup.MagFilter, setup.MinFilter);
    smpDesc.AddressU = d3d11Types::asTextureAddressMode(setup.WrapU);
    smpDesc.AddressV = d3d11Types::asTextureAddressMode(setup.WrapV);
    smpDesc.AddressW = d3d11Types::asTextureAddressMode(setup.WrapW);
    smpDesc.MipLODBias = 0.0f;
    smpDesc.MaxAnisotropy = 1;
    smpDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    smpDesc.BorderColor[0] = 1.0f;
    smpDesc.BorderColor[1] = 1.0f;
    smpDesc.BorderColor[2] = 1.0f;
    smpDesc.BorderColor[3] = 1.0f;
    smpDesc.MinLOD = -D3D11_FLOAT32_MAX;
    smpDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = this->d3d11Device->CreateSamplerState(&smpDesc, &tex.d3d11SamplerState);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(nullptr != tex.d3d11SamplerState);

    // setup texture attributes
    TextureAttrs attrs;
    attrs.Locator = setup.Locator;
    attrs.Type = setup.Type;
    attrs.ColorFormat = setup.ColorFormat;
    attrs.TextureUsage = Usage::Immutable;
    attrs.Width = setup.Width;
    attrs.Height = setup.Height;
    attrs.NumMipMaps = setup.NumMipMaps;
    tex.textureAttrs = attrs;

    return ResourceState::Valid;
}

} // namespace _priv
} // namespace Oryol