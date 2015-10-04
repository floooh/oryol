//------------------------------------------------------------------------------
//  d3d11TextureFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11TextureFactory.h"
#include "Gfx/d3d11/d3d11_impl.h"
#include "Gfx/d3d11/d3d11types.h"
#include "Gfx/Resource/resourcePools.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Core/displayMgr.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d11TextureFactory::d3d11TextureFactory() :
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
d3d11TextureFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptrs;
    this->d3d11Device = this->pointers.renderer->d3d11Device;
}

//------------------------------------------------------------------------------
void
d3d11TextureFactory::Discard() {
    o_assert_dbg(this->isValid);

    this->isValid = false;
    this->pointers = gfxPointers();
    this->d3d11Device = nullptr;
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

    this->pointers.renderer->invalidateTextureState();

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
    o_assert_dbg(nullptr != this->d3d11Device);
    o_assert_dbg(nullptr == tex.d3d11Texture2D);
    o_assert_dbg(nullptr == tex.d3d11ShaderResourceView);
    o_assert_dbg(nullptr == tex.d3d11RenderTargetView);
    o_assert_dbg(nullptr == tex.d3d11SamplerState);
    o_assert_dbg(nullptr == tex.d3d11DepthBufferTexture);
    o_assert_dbg(nullptr == tex.d3d11DepthStencilView);

    const TextureSetup& setup = tex.Setup;
    o_assert_dbg(setup.ShouldSetupAsRenderTarget());
    o_assert_dbg(setup.NumMipMaps == 1);
    o_assert_dbg(setup.Type == TextureType::Texture2D);
    o_assert_dbg(PixelFormat::IsValidRenderTargetColorFormat(setup.ColorFormat));

    // get size of new render target
    int32 width, height;
    texture* sharedDepthProvider = nullptr;
    if (setup.IsRelSizeRenderTarget()) {
        const DisplayAttrs& dispAttrs = this->pointers.displayMgr->GetDisplayAttrs();
        width = int32(dispAttrs.FramebufferWidth * setup.RelWidth);
        height = int32(dispAttrs.FramebufferHeight * setup.RelHeight);
    }
    else if (setup.HasSharedDepth()) {
        // a shared-depth-buffer render target, obtain width and height
        // from the original render target
        sharedDepthProvider = this->pointers.texturePool->Lookup(setup.DepthRenderTarget);
        o_assert_dbg(nullptr != sharedDepthProvider);
        width = sharedDepthProvider->textureAttrs.Width;
        height = sharedDepthProvider->textureAttrs.Height;
    }
    else {
        width = setup.Width;
        height = setup.Height;
    }
    o_assert_dbg((width > 0) && (height > 0));

    // create the color texture
    D3D11_TEXTURE2D_DESC texDesc;
    Memory::Clear(&texDesc, sizeof(texDesc));
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = d3d11Types::asRenderTargetFormat(setup.ColorFormat);
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;
    HRESULT hr = this->d3d11Device->CreateTexture2D(&texDesc, nullptr, &tex.d3d11Texture2D);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(nullptr != tex.d3d11Texture2D);

    // create shader-resource-view object
    tex.d3d11ShaderResourceView = this->createShaderResourceView(tex, tex.d3d11Texture2D, &texDesc);
    o_assert_dbg(tex.d3d11ShaderResourceView);

    // create render-target-view object
    tex.d3d11RenderTargetView = this->createRenderTargetView(tex, tex.d3d11Texture2D, &texDesc);
    o_assert_dbg(tex.d3d11RenderTargetView);
    
    // create render-target texture sampler object
    tex.d3d11SamplerState = this->createSamplerState(tex);
    o_assert_dbg(tex.d3d11SamplerState);

    // handle depth buffer
    if (setup.HasDepth()) {
        if (setup.HasSharedDepth()) {
            // shared depth, simply take over existing objects, and bump refcount
            o_assert_dbg(sharedDepthProvider->d3d11DepthBufferTexture);
            o_assert_dbg(sharedDepthProvider->d3d11DepthStencilView);
            tex.d3d11DepthBufferTexture = sharedDepthProvider->d3d11DepthBufferTexture;
            tex.d3d11DepthStencilView = sharedDepthProvider->d3d11DepthStencilView;
            tex.d3d11DepthBufferTexture->AddRef();
            tex.d3d11DepthStencilView->AddRef();
        }
        else {
            // create depth-buffer-texture
            o_assert_dbg(PixelFormat::IsValidRenderTargetDepthFormat(setup.DepthFormat));
            o_assert_dbg(setup.DepthFormat != PixelFormat::None);
            D3D11_TEXTURE2D_DESC depthDesc = texDesc;
            depthDesc.Format = d3d11Types::asRenderTargetFormat(setup.DepthFormat);
            depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            hr = this->d3d11Device->CreateTexture2D(&depthDesc, nullptr, &tex.d3d11DepthBufferTexture);
            o_assert(SUCCEEDED(hr));
            o_assert_dbg(nullptr != tex.d3d11DepthBufferTexture);

            // create depth-stencil-view
            tex.d3d11DepthStencilView = this->createDepthStencilView(tex, tex.d3d11DepthBufferTexture, &depthDesc);
            o_assert_dbg(nullptr != tex.d3d11DepthStencilView);
        }
    }

    // setup texture attrs and set on texture
    TextureAttrs attrs;
    attrs.Locator = setup.Locator;
    attrs.Type = TextureType::Texture2D;
    attrs.ColorFormat = setup.ColorFormat;
    attrs.DepthFormat = setup.DepthFormat;
    attrs.TextureUsage = Usage::Immutable;
    attrs.Width = width;
    attrs.Height = height;
    attrs.NumMipMaps = 1;
    attrs.IsRenderTarget = true;
    attrs.HasDepthBuffer = setup.HasDepth();
    attrs.HasSharedDepthBuffer = setup.HasSharedDepth();
    tex.textureAttrs = attrs;

    return ResourceState::Valid;
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

    if (setup.Type == TextureType::Texture3D) {
        o_warn("d3d11TextureFactory: 3d textures not yet implemented\n");
        return ResourceState::Failed;
    }
    DXGI_FORMAT d3d11PixelFormat = d3d11Types::asTextureFormat(setup.ColorFormat);
    if (DXGI_FORMAT_UNKNOWN == d3d11PixelFormat) {
        o_warn("d3d11TextureFactory: texture pixel format not supported in D3D11\n");
        return ResourceState::Failed;     
    }

    // create the 2D or Cube texture object with data
    D3D11_TEXTURE2D_DESC texDesc;
    Memory::Clear(&texDesc, sizeof(texDesc));
    texDesc.Width = setup.Width;
    texDesc.Height = setup.Height;
    texDesc.MipLevels = setup.NumMipMaps;
    texDesc.ArraySize = setup.Type == TextureType::TextureCube ? 6 : 1;
    texDesc.Format = d3d11PixelFormat;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_IMMUTABLE;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = setup.Type == TextureType::TextureCube ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;

    const int32 maxNumSubResourceData = GfxConfig::MaxNumTextureFaces * GfxConfig::MaxNumTextureMipMaps;
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

    // create d3d11 shader-resource-view object
    tex.d3d11ShaderResourceView = this->createShaderResourceView(tex, tex.d3d11Texture2D, &texDesc);
    o_assert_dbg(tex.d3d11ShaderResourceView);

    // create sampler object (note: samplers with identical state are automatically shared by D3D11)
    tex.d3d11SamplerState = this->createSamplerState(tex);
    o_assert_dbg(tex.d3d11SamplerState);

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

//------------------------------------------------------------------------------
ID3D11ShaderResourceView*
d3d11TextureFactory::createShaderResourceView(const texture& tex, ID3D11Texture2D* d3d11Tex, const D3D11_TEXTURE2D_DESC* texDesc) {
    D3D11_SHADER_RESOURCE_VIEW_DESC shdResViewDesc;
    Memory::Clear(&shdResViewDesc, sizeof(shdResViewDesc));
    shdResViewDesc.Format = texDesc->Format;
    if (tex.Setup.Type == TextureType::TextureCube) {
        shdResViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
        shdResViewDesc.TextureCube.MostDetailedMip = 0;
        shdResViewDesc.TextureCube.MipLevels = tex.Setup.NumMipMaps;
    }
    else {
        shdResViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        shdResViewDesc.Texture2D.MostDetailedMip = 0;
        shdResViewDesc.Texture2D.MipLevels = tex.Setup.NumMipMaps;
    }
    ID3D11ShaderResourceView* d3d11ShaderResourceView = nullptr;
    HRESULT hr = this->d3d11Device->CreateShaderResourceView(d3d11Tex, &shdResViewDesc, &d3d11ShaderResourceView);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(nullptr != d3d11ShaderResourceView);
    return d3d11ShaderResourceView;
}

//------------------------------------------------------------------------------
ID3D11RenderTargetView*
d3d11TextureFactory::createRenderTargetView(const texture& tex, ID3D11Texture2D* d3d11Tex, const D3D11_TEXTURE2D_DESC* texDesc) {
    D3D11_RENDER_TARGET_VIEW_DESC rtViewDesc;
    Memory::Clear(&rtViewDesc, sizeof(rtViewDesc));
    rtViewDesc.Format = texDesc->Format;
    rtViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtViewDesc.Texture2D.MipSlice = 0;
    ID3D11RenderTargetView* d3d11RenderTargetView = nullptr;
    HRESULT hr = this->d3d11Device->CreateRenderTargetView(d3d11Tex, &rtViewDesc, &d3d11RenderTargetView);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(nullptr != d3d11RenderTargetView);
    return d3d11RenderTargetView;
}

//------------------------------------------------------------------------------
ID3D11DepthStencilView*
d3d11TextureFactory::createDepthStencilView(const texture& tex, ID3D11Texture2D* d3d11Tex, const D3D11_TEXTURE2D_DESC* texDesc) {
    D3D11_DEPTH_STENCIL_VIEW_DESC dsViewDesc;
    Memory::Clear(&dsViewDesc, sizeof(dsViewDesc));
    dsViewDesc.Format = texDesc->Format;
    dsViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsViewDesc.Texture2D.MipSlice = 0;
    ID3D11DepthStencilView* d3d11DepthStencilView = nullptr;
    HRESULT hr = this->d3d11Device->CreateDepthStencilView(d3d11Tex, &dsViewDesc, &d3d11DepthStencilView);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(d3d11DepthStencilView);
    return d3d11DepthStencilView;
}

//------------------------------------------------------------------------------
ID3D11SamplerState*
d3d11TextureFactory::createSamplerState(const texture& tex) {
    D3D11_SAMPLER_DESC smpDesc;
    Memory::Clear(&smpDesc, sizeof(smpDesc));
    smpDesc.Filter = d3d11Types::asSamplerFilter(tex.Setup.Sampler.MagFilter, tex.Setup.Sampler.MinFilter);
    smpDesc.AddressU = d3d11Types::asTextureAddressMode(tex.Setup.Sampler.WrapU);
    smpDesc.AddressV = d3d11Types::asTextureAddressMode(tex.Setup.Sampler.WrapV);
    smpDesc.AddressW = d3d11Types::asTextureAddressMode(tex.Setup.Sampler.WrapW);
    smpDesc.MipLODBias = 0.0f;
    smpDesc.MaxAnisotropy = 1;
    smpDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    smpDesc.BorderColor[0] = 1.0f;
    smpDesc.BorderColor[1] = 1.0f;
    smpDesc.BorderColor[2] = 1.0f;
    smpDesc.BorderColor[3] = 1.0f;
    smpDesc.MinLOD = -D3D11_FLOAT32_MAX;
    smpDesc.MaxLOD = D3D11_FLOAT32_MAX;
    ID3D11SamplerState* d3d11SamplerState = nullptr;
    HRESULT hr = this->d3d11Device->CreateSamplerState(&smpDesc, &d3d11SamplerState);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(nullptr != d3d11SamplerState);
    return d3d11SamplerState;
}

} // namespace _priv
} // namespace Oryol