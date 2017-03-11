//------------------------------------------------------------------------------
//  d3d11TextureFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11TextureFactory.h"
#include "Gfx/d3d11/d3d11_impl.h"
#include "Gfx/d3d11/d3d11types.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Resource/resource.h"
#include <algorithm> // for std::max

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d11TextureFactory::~d3d11TextureFactory() {
    o_assert_dbg(!this->isValid);
    o_assert_dbg(!this->subResourceData);
}

//------------------------------------------------------------------------------
void
d3d11TextureFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptrs;
    this->d3d11Device = this->pointers.renderer->d3d11Device;
    this->subResourceData = (D3D11_SUBRESOURCE_DATA*) Memory::Alloc(sizeof(D3D11_SUBRESOURCE_DATA) * maxNumSubResourceData);
}

//------------------------------------------------------------------------------
void
d3d11TextureFactory::Discard() {
    o_assert_dbg(this->isValid);
    o_assert_dbg(this->subResourceData);

    this->isValid = false;
    this->pointers = gfxPointers();
    this->d3d11Device = nullptr;
    Memory::Free(this->subResourceData);
    this->subResourceData = nullptr;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11TextureFactory::SetupResource(texture& tex, const void* data, int size) {
    o_assert_dbg(this->isValid);
    return this->createTexture(tex, data, size);
}

//------------------------------------------------------------------------------
void
d3d11TextureFactory::DestroyResource(texture& tex) {
    o_assert_dbg(this->isValid);

    this->pointers.renderer->invalidateTextureState();

    if (tex.d3d11Texture2D) {
        tex.d3d11Texture2D->Release();
    }
    if (tex.d3d11Texture3D) {
        tex.d3d11Texture3D->Release();
    }
    if (tex.d3d11ShaderResourceView) {
        tex.d3d11ShaderResourceView->Release();
    }
    if (tex.d3d11SamplerState) {
        tex.d3d11SamplerState->Release();
    }
    if (tex.d3d11DepthStencilTexture) {
        tex.d3d11DepthStencilTexture->Release();
    }
    if (tex.d3d11MSAATexture2D) {
        tex.d3d11MSAATexture2D->Release();
    }
    tex.Clear();
}

//------------------------------------------------------------------------------
void
d3d11TextureFactory::setupTextureAttrs(texture& tex) {
    TextureAttrs attrs;
    attrs.Locator = tex.Setup.Locator;
    attrs.Type = tex.Setup.Type;
    attrs.ColorFormat = tex.Setup.ColorFormat;
    attrs.DepthFormat = tex.Setup.DepthFormat;
    attrs.SampleCount = tex.Setup.SampleCount;
    attrs.TextureUsage = tex.Setup.TextureUsage;
    attrs.Width = tex.Setup.Width;
    attrs.Height = tex.Setup.Height;
    attrs.Depth = tex.Setup.Depth;
    attrs.NumMipMaps = tex.Setup.NumMipMaps;
    attrs.IsRenderTarget = tex.Setup.RenderTarget;
    attrs.HasDepthBuffer = tex.Setup.HasDepth();
    tex.textureAttrs = attrs;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11TextureFactory::createTexture(texture& tex, const void* data, int size) {
    o_assert_dbg(this->d3d11Device);
    o_assert_dbg(this->subResourceData);
    o_assert_dbg(!tex.d3d11Texture2D);
    o_assert_dbg(!tex.d3d11Texture3D);
    o_assert_dbg(!tex.d3d11ShaderResourceView);
    o_assert_dbg(!tex.d3d11SamplerState);
    o_assert_dbg(!tex.d3d11DepthStencilTexture);
    o_assert_dbg(!tex.d3d11MSAATexture2D);

    const TextureSetup& setup = tex.Setup;
    o_assert_dbg(!setup.ShouldSetupFromNativeTexture());

    // subresourcedata array if initial data is provided
    D3D11_SUBRESOURCE_DATA* pInitialData = nullptr;
    if (data) {
        const uint8_t* srcPtr = (const uint8_t*)data;
        const int numSlices = setup.Type==TextureType::TextureCube ? setup.ImageData.NumFaces:(setup.Type==TextureType::TextureArray ? setup.Depth:1);
        const int numMipMaps = setup.ImageData.NumMipMaps;
        int subResourceDataIndex = 0;
        for (int sliceIndex = 0; sliceIndex < numSlices; sliceIndex++) {
            for (int mipIndex = 0; mipIndex < numMipMaps; mipIndex++, subResourceDataIndex++) {
                o_assert_dbg(subResourceDataIndex < maxNumSubResourceData);
                D3D11_SUBRESOURCE_DATA& subResData = this->subResourceData[subResourceDataIndex];
                subResData.pSysMem = srcPtr + setup.ImageData.Offsets[sliceIndex][mipIndex]; 
                const int mipWidth = std::max(setup.Width >> mipIndex, 1);
                const int mipHeight = std::max(setup.Height >> mipIndex, 1);
                subResData.SysMemPitch = PixelFormat::RowPitch(setup.ColorFormat, mipWidth);
                if (setup.Type == TextureType::Texture3D) {
                    const int mipDepth = std::max(setup.Depth >> mipIndex, 1);
                    subResData.SysMemSlicePitch = PixelFormat::ImagePitch(setup.ColorFormat, mipWidth, mipHeight);
                }
                else {
                    subResData.SysMemSlicePitch = 0;
                }
            }
        }
        pInitialData = this->subResourceData;
    }

    // create the color texture
    HRESULT hr;
    if (setup.Type != TextureType::Texture3D) {
        // setup texture desc for 2D texture
        D3D11_TEXTURE2D_DESC texDesc = { };
        texDesc.Width = setup.Width;
        texDesc.Height = setup.Height;
        texDesc.MipLevels = setup.NumMipMaps;
        if (TextureType::TextureArray == setup.Type) {
            texDesc.ArraySize = setup.Depth;
        }
        else if (TextureType::TextureCube == setup.Type) {
            texDesc.ArraySize = 6;
        }
        else {
            texDesc.ArraySize = 1;
        }
        if (setup.RenderTarget) {
            texDesc.Format = tex.d3d11ColorFormat = d3d11Types::asRenderTargetFormat(setup.ColorFormat);
            o_assert2_dbg(texDesc.Format != DXGI_FORMAT_UNKNOWN, "Invalid render target pixel format!");
            texDesc.Usage = D3D11_USAGE_DEFAULT;
            texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET;
            texDesc.CPUAccessFlags = 0;
        }
        else {
            texDesc.Format = tex.d3d11ColorFormat = d3d11Types::asTextureFormat(setup.ColorFormat);
            if (DXGI_FORMAT_UNKNOWN == texDesc.Format) {
                o_warn("d3d11TextureFactory: texture pixel format not supported in D3D11\n");
                return ResourceState::Failed;
            }
            texDesc.Usage = d3d11Types::asResourceUsage(setup.TextureUsage);
            texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            texDesc.CPUAccessFlags = d3d11Types::asResourceCPUAccessFlag(setup.TextureUsage);
        }
        texDesc.SampleDesc.Count = 1; 
        texDesc.SampleDesc.Quality = 0;
        texDesc.MiscFlags = setup.Type == TextureType::TextureCube ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;
        hr = this->d3d11Device->CreateTexture2D(&texDesc, pInitialData, &tex.d3d11Texture2D);
        o_assert(SUCCEEDED(hr));
        
        // also create an MSAA texture?
        if (setup.SampleCount > 1) {
            texDesc.SampleDesc.Count = setup.SampleCount;
            texDesc.SampleDesc.Quality = 0;
            hr = this->d3d11Device->CreateTexture2D(&texDesc, pInitialData, &tex.d3d11MSAATexture2D);
            o_assert(SUCCEEDED(hr));
        }
        
        // shader-resource-view (always on the non-MSAA texture)
        tex.d3d11ShaderResourceView = this->createShaderResourceView(tex, tex.d3d11Texture2D, texDesc.Format);
        o_assert(tex.d3d11ShaderResourceView);
    }
    else {
        // 3D texture
        D3D11_TEXTURE3D_DESC texDesc = { };
        texDesc.Width = setup.Width;
        texDesc.Height = setup.Height;
        texDesc.Depth = setup.Depth;
        texDesc.MipLevels = setup.NumMipMaps;
        if (setup.RenderTarget) {
            texDesc.Format = tex.d3d11ColorFormat = d3d11Types::asRenderTargetFormat(setup.ColorFormat);
            o_assert2_dbg(texDesc.Format != DXGI_FORMAT_UNKNOWN, "Invalid render target color pixel format!");
            texDesc.Usage = D3D11_USAGE_DEFAULT;
            texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
            texDesc.CPUAccessFlags = 0;
        }
        else {
            texDesc.Format = tex.d3d11ColorFormat = d3d11Types::asTextureFormat(setup.ColorFormat);
            o_assert2_dbg(texDesc.Format != DXGI_FORMAT_UNKNOWN, "Invalid texture pixel format!");
            texDesc.Usage = d3d11Types::asResourceUsage(setup.TextureUsage);
            texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            texDesc.CPUAccessFlags = d3d11Types::asResourceCPUAccessFlag(setup.TextureUsage);
        }
        texDesc.MiscFlags = 0;
        hr = this->d3d11Device->CreateTexture3D(&texDesc, pInitialData, &tex.d3d11Texture3D);
        o_assert(SUCCEEDED(hr));
        tex.d3d11ShaderResourceView = this->createShaderResourceView(tex, tex.d3d11Texture3D, texDesc.Format);
        o_assert(tex.d3d11ShaderResourceView);
    }

    // optional depth-stencil-buffer texture
    if (setup.RenderTarget && (setup.DepthFormat != PixelFormat::InvalidPixelFormat)) {
        // create depth-buffer-texture
        D3D11_TEXTURE2D_DESC dsDesc = {};
        dsDesc.Width = setup.Width;
        dsDesc.Height = setup.Height;
        dsDesc.MipLevels = setup.NumMipMaps;
        dsDesc.ArraySize = 1;
        dsDesc.Format = d3d11Types::asRenderTargetFormat(setup.DepthFormat);
        o_assert2_dbg(dsDesc.Format != DXGI_FORMAT_UNKNOWN, "Invalid render target depth pixel format!");
        dsDesc.Usage = D3D11_USAGE_DEFAULT;
        dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        dsDesc.CPUAccessFlags = 0;
        dsDesc.SampleDesc.Count = setup.SampleCount;
        dsDesc.SampleDesc.Quality = 0;
        dsDesc.MiscFlags = 0;
        hr = this->d3d11Device->CreateTexture2D(&dsDesc, nullptr, &tex.d3d11DepthStencilTexture);
        o_assert(SUCCEEDED(hr));
        o_assert_dbg(nullptr != tex.d3d11DepthStencilTexture);
    }

    // create sampler object
    tex.d3d11SamplerState = this->createSamplerState(tex);
    o_assert(tex.d3d11SamplerState);

    // fill texture attributes
    o_assert_dbg(DXGI_FORMAT_UNKNOWN != tex.d3d11ColorFormat);
    this->setupTextureAttrs(tex);

    // and done
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
ID3D11ShaderResourceView*
d3d11TextureFactory::createShaderResourceView(const texture& tex, ID3D11Resource* d3d11Tex, DXGI_FORMAT d3d11Format) {
    D3D11_SHADER_RESOURCE_VIEW_DESC shdResViewDesc = { };
    shdResViewDesc.Format = d3d11Format;
    switch (tex.Setup.Type) {
        case TextureType::Texture2D:
            shdResViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            shdResViewDesc.Texture2D.MostDetailedMip = 0;
            shdResViewDesc.Texture2D.MipLevels = tex.Setup.NumMipMaps;
            break;
        case TextureType::TextureCube:
            shdResViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
            shdResViewDesc.TextureCube.MostDetailedMip = 0;
            shdResViewDesc.TextureCube.MipLevels = tex.Setup.NumMipMaps;
            break;
        case TextureType::Texture3D:
            shdResViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
            shdResViewDesc.Texture3D.MostDetailedMip = 0;
            shdResViewDesc.Texture3D.MipLevels = tex.Setup.NumMipMaps;
            break;
        case TextureType::TextureArray:
            shdResViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            shdResViewDesc.Texture2DArray.MostDetailedMip = 0;
            shdResViewDesc.Texture2DArray.MipLevels = tex.Setup.NumMipMaps;
            shdResViewDesc.Texture2DArray.FirstArraySlice = 0;
            shdResViewDesc.Texture2DArray.ArraySize = tex.Setup.Depth;
            break;
        default:
            o_error("d3d11TextureFactory::createShaderResourceView: invalid texture type!\n");
            break;
    }
    ID3D11ShaderResourceView* d3d11ShaderResourceView = nullptr;
    HRESULT hr = this->d3d11Device->CreateShaderResourceView(d3d11Tex, &shdResViewDesc, &d3d11ShaderResourceView);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(nullptr != d3d11ShaderResourceView);
    return d3d11ShaderResourceView;
}

//------------------------------------------------------------------------------
ID3D11SamplerState*
d3d11TextureFactory::createSamplerState(const texture& tex) {
    D3D11_SAMPLER_DESC smpDesc = { };
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
