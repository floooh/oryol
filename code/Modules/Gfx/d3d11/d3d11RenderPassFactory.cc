//------------------------------------------------------------------------------
//  d3d11RenderPassFactory.cc
//------------------------------------------------------------------------------
#include "Core/Assertion.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Resource/resource.h"
#include "d3d11RenderPassFactory.h"
#include "d3d11_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d11RenderPassFactory::~d3d11RenderPassFactory() {
    o_assert_dbg(!this->d3d11Device);
}

//------------------------------------------------------------------------------
void
d3d11RenderPassFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->d3d11Device);
    renderPassFactoryBase::Setup(ptrs);
    this->d3d11Device = this->pointers.renderer->d3d11Device;    
}

//------------------------------------------------------------------------------
void
d3d11RenderPassFactory::Discard() {
    o_assert_dbg(this->d3d11Device);
    this->d3d11Device = nullptr;
    renderPassFactoryBase::Discard();
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11RenderPassFactory::SetupResource(renderPass& rp) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(this->d3d11Device);

    renderPassFactoryBase::SetupResource(rp);
    o_assert_dbg(rp.colorTextures[0]);
    const bool isMSAA = rp.colorTextures[0]->Setup.SampleCount > 1;

    // create render-target-view objects
    for (int i = 0; i < GfxConfig::MaxNumColorAttachments; i++) {
        o_assert_dbg(nullptr == rp.d3d11RenderTargetViews[i]);
        texture* colorTex = rp.colorTextures[i];
        if (colorTex) {
            D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = { };
            ID3D11Resource* d3d11Res = nullptr;
            if (colorTex->d3d11MSAATexture2D) {
                d3d11Res = colorTex->d3d11MSAATexture2D;
                D3D11_TEXTURE2D_DESC texDesc = { };
                colorTex->d3d11MSAATexture2D->GetDesc(&texDesc);
                rtvDesc.Format = texDesc.Format;
            }
            else if (colorTex->d3d11Texture2D) {
                d3d11Res = colorTex->d3d11Texture2D;
                D3D11_TEXTURE2D_DESC texDesc = { };
                colorTex->d3d11Texture2D->GetDesc(&texDesc);
                rtvDesc.Format = texDesc.Format;
            }
            else {
                d3d11Res = colorTex->d3d11Texture3D;
                D3D11_TEXTURE3D_DESC texDesc = { };
                colorTex->d3d11Texture3D->GetDesc(&texDesc);
                rtvDesc.Format = texDesc.Format;
            }
            o_assert_dbg(d3d11Res);
            switch (colorTex->Setup.Type) {
                case TextureType::Texture2D:
                    if (isMSAA) {
                        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
                    }
                    else {
                        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
                        rtvDesc.Texture2D.MipSlice = rp.Setup.ColorAttachments[i].Level;
                    }
                    break;
                case TextureType::TextureCube:
                    if (isMSAA) {
                        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
                        rtvDesc.Texture2DMSArray.FirstArraySlice = rp.Setup.ColorAttachments[i].Face;
                        rtvDesc.Texture2DMSArray.ArraySize = 1;
                    }
                    else {
                        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
                        rtvDesc.Texture2DArray.MipSlice = rp.Setup.ColorAttachments[i].Level;
                        rtvDesc.Texture2DArray.FirstArraySlice = rp.Setup.ColorAttachments[i].Face;
                        rtvDesc.Texture2DArray.ArraySize = 1;
                    }
                    break;
                case TextureType::Texture3D:
                    o_assert_dbg(!isMSAA);
                    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
                    rtvDesc.Texture3D.MipSlice = rp.Setup.ColorAttachments[i].Level;
                    rtvDesc.Texture3D.FirstWSlice = rp.Setup.ColorAttachments[i].Layer;
                    rtvDesc.Texture3D.WSize = 1;
                    break;
                case TextureType::TextureArray:
                    if (isMSAA) {
                        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
                        rtvDesc.Texture2DMSArray.FirstArraySlice = rp.Setup.ColorAttachments[i].Layer;
                        rtvDesc.Texture2DMSArray.ArraySize = 1;
                    }
                    else {
                        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
                        rtvDesc.Texture2DArray.MipSlice = rp.Setup.ColorAttachments[i].Level;
                        rtvDesc.Texture2DArray.FirstArraySlice = rp.Setup.ColorAttachments[i].Layer;
                        rtvDesc.Texture2DArray.ArraySize = 1;
                    }
                default:
                    o_error("d3d11RenderPassFactory: invalid texture type\n");
                    break;
            }
            HRESULT hr = this->d3d11Device->CreateRenderTargetView(d3d11Res, &rtvDesc, &(rp.d3d11RenderTargetViews[i]));
            o_assert(SUCCEEDED(hr) && rp.d3d11RenderTargetViews[i]);
        } 
    }

    // create optional depth-stencil-view object
    if (rp.depthStencilTexture) {
        o_assert_dbg(nullptr == rp.d3d11DepthStencilView);
        texture* dsTex = rp.depthStencilTexture;
        o_assert_dbg(dsTex->d3d11DepthStencilTexture);
        D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc = { };
        D3D11_TEXTURE2D_DESC texDesc = { };
        dsTex->d3d11DepthStencilTexture->GetDesc(&texDesc);
        dsDesc.Format = texDesc.Format;
        dsDesc.Flags = 0;
        if (isMSAA) {
            dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
            dsDesc.Texture2D.MipSlice = 0;
        }
        else {
            dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        } 
        HRESULT hr = this->d3d11Device->CreateDepthStencilView(dsTex->d3d11DepthStencilTexture, &dsDesc, &rp.d3d11DepthStencilView);
        o_assert(SUCCEEDED(hr) && rp.d3d11DepthStencilView); 
    }
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
d3d11RenderPassFactory::DestroyResource(renderPass& rp) {
    o_assert_dbg(this->isValid);

    for (int i = 0; i < GfxConfig::MaxNumColorAttachments; i++) {
        if (rp.d3d11RenderTargetViews[i]) {
            rp.d3d11RenderTargetViews[i]->Release();
        }
    }
    if (rp.d3d11DepthStencilView) {
        rp.d3d11DepthStencilView->Release();
    }

    renderPassFactoryBase::DestroyResource(rp);
}

} // namespace _priv
} // namespace Oryol
