//------------------------------------------------------------------------------
//  d3d11DisplayMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11DisplayMgr.h"
#include "d3d11Types.h"
#include "Gfx/private/renderer.h"

#ifndef UNICODE
#define UNICODE
#endif
#include "d3d11_impl.h"

namespace Oryol {
namespace _priv {

static DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;

//------------------------------------------------------------------------------
d3d11DisplayMgr::~d3d11DisplayMgr() {
    if (this->IsDisplayValid()) {
        this->DiscardDisplay();
    }
}   

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::SetupDisplay(const GfxSetup& setup, const gfxPointers& ptrs) {
    o_assert(!this->IsDisplayValid());
    Memory::Clear(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
    winDisplayMgr::SetupDisplay(setup, ptrs, " (D3D11)");
    this->createDeviceAndSwapChain();
    const DisplayAttrs& attrs = this->displayAttrs;
    this->createDefaultRenderTarget(attrs.FramebufferWidth, attrs.FramebufferHeight);
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::DiscardDisplay() {
    o_assert(this->IsDisplayValid());
    this->destroyDefaultRenderTarget();
    this->destroyDeviceAndSwapChain();
    winDisplayMgr::DiscardDisplay();
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::Present() {
    o_assert_dbg(this->dxgiSwapChain);
    this->dxgiSwapChain->Present(this->gfxSetup.SwapInterval, 0);
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::createDeviceAndSwapChain() {
    o_assert_dbg(nullptr == this->d3d11Device);
    o_assert_dbg(nullptr == this->d3d11DeviceContext);
    o_assert_dbg(nullptr == this->dxgiSwapChain);
    o_assert_dbg(0 != this->hwnd);

    int createFlags = D3D11_CREATE_DEVICE_SINGLETHREADED;
    #if ORYOL_DEBUG
        createFlags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif

    Memory::Clear(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
    dxgiSwapChainDesc.BufferDesc.Width = this->displayAttrs.FramebufferWidth;
    dxgiSwapChainDesc.BufferDesc.Height = this->displayAttrs.FramebufferHeight;
    dxgiSwapChainDesc.BufferDesc.Format = d3d11Types::asSwapChainFormat(this->gfxSetup.ColorFormat);
    dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    dxgiSwapChainDesc.OutputWindow = this->hwnd;
    dxgiSwapChainDesc.Windowed = this->gfxSetup.Windowed;
    dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    dxgiSwapChainDesc.BufferCount = 1;
    dxgiSwapChainDesc.SampleDesc.Count = this->gfxSetup.SampleCount;
    dxgiSwapChainDesc.SampleDesc.Quality = this->gfxSetup.SampleCount > 1 ? D3D11_STANDARD_MULTISAMPLE_PATTERN : 0;
    dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    dxgiSwapChainDesc.Flags = 0;

    D3D_FEATURE_LEVEL d3dFeatureLevel;
    HRESULT hr = ::D3D11CreateDeviceAndSwapChain(
        NULL,                           // pAdapter (use default adapter)
        D3D_DRIVER_TYPE_HARDWARE,       // DriverType
        NULL,                           // Software
        createFlags,                    // Flags
        NULL,                           // pFeatureLevels
        0,                              // FeatureLevels
        D3D11_SDK_VERSION,              // SDKVersion
        &dxgiSwapChainDesc,             // pSwapChainDesc
        &this->dxgiSwapChain,           // ppSwapChain
        &this->d3d11Device,             // ppDevice
        &d3dFeatureLevel,               // pFeatureLevel
        &this->d3d11DeviceContext);     // ppImmediateContext
    o_assert2(SUCCEEDED(hr), "Failed to create D3D11 device and swap chain!\n");
    o_assert(this->d3d11Device);
    o_assert(this->d3d11DeviceContext);
    o_assert(this->dxgiSwapChain);
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::destroyDeviceAndSwapChain() {
    if (this->dxgiSwapChain) {
        this->dxgiSwapChain->Release();
        this->dxgiSwapChain = nullptr;
    }
    if (this->d3d11Device) {
        this->d3d11Device->Release();
        this->d3d11Device = nullptr;
    }
    if (this->d3d11DeviceContext) {
        this->d3d11DeviceContext->Release();
        this->d3d11DeviceContext = nullptr;
    }
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::createDefaultRenderTarget(int width, int height) {
    o_assert_dbg(this->d3d11Device);
    o_assert_dbg(this->dxgiSwapChain);
    o_assert_dbg(nullptr == this->d3d11RenderTarget);
    o_assert_dbg(nullptr == this->d3d11RenderTargetView);
    o_assert_dbg(nullptr == this->d3d11DepthStencilBuffer);
    o_assert_dbg(nullptr == this->d3d11DepthStencilView);
    HRESULT hr;

    // setup color buffer (obtain from swap chain and create render target view)
    hr = this->dxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &this->d3d11RenderTarget);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(this->d3d11RenderTarget);
    hr = this->d3d11Device->CreateRenderTargetView(this->d3d11RenderTarget, NULL, &this->d3d11RenderTargetView);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(this->d3d11RenderTargetView);

    // setup depth/stencil buffer (if required)
    if (PixelFormat::None != this->gfxSetup.DepthFormat) {
        
        D3D11_TEXTURE2D_DESC depthStencilDesc = { };
        depthStencilDesc.Width = width;
        depthStencilDesc.Height = height;
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.ArraySize = 1;
        depthStencilDesc.Format = d3d11Types::asRenderTargetFormat(this->gfxSetup.DepthFormat);
        depthStencilDesc.SampleDesc = dxgiSwapChainDesc.SampleDesc;
        depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
        depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthStencilDesc.CPUAccessFlags = 0;
        depthStencilDesc.MiscFlags = 0;
        hr = this->d3d11Device->CreateTexture2D(&depthStencilDesc, nullptr, &this->d3d11DepthStencilBuffer);
        o_assert(SUCCEEDED(hr));
        o_assert_dbg(this->d3d11DepthStencilBuffer);

        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = { };
        dsvDesc.Format = depthStencilDesc.Format;
        if (this->gfxSetup.SampleCount > 1) {
            dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
        }
        else {
            dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        }
        dsvDesc.Texture2D.MipSlice = 0;
        hr = this->d3d11Device->CreateDepthStencilView(this->d3d11DepthStencilBuffer, &dsvDesc, &this->d3d11DepthStencilView);
        o_assert(SUCCEEDED(hr));
        o_assert_dbg(this->d3d11DepthStencilView);
    }
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::destroyDefaultRenderTarget() {
    if (this->d3d11RenderTarget) {
        this->d3d11RenderTarget->Release();
        this->d3d11RenderTarget = nullptr;
    }
    if (this->d3d11RenderTargetView) {
        this->d3d11RenderTargetView->Release();
        this->d3d11RenderTargetView = nullptr;
    }
    if (this->d3d11DepthStencilBuffer) {
        this->d3d11DepthStencilBuffer->Release();
        this->d3d11DepthStencilBuffer = nullptr;
    }
    if (this->d3d11DepthStencilView) {
        this->d3d11DepthStencilView->Release();
        this->d3d11DepthStencilView = nullptr;
    }
}

//------------------------------------------------------------------------------
void
d3d11DisplayMgr::onWindowDidResize() {

    // resize the DXGI framebuffer (this requires that all state is unbound)
    if (this->dxgiSwapChain) {

        const int newWidth = this->displayAttrs.FramebufferWidth;
        const int newHeight = this->displayAttrs.FramebufferHeight;

        this->pointers.renderer->resetStateCache();
        this->destroyDefaultRenderTarget();
        DXGI_FORMAT d3d11Fmt = d3d11Types::asSwapChainFormat(this->gfxSetup.ColorFormat);
        HRESULT hr = this->dxgiSwapChain->ResizeBuffers(1, newWidth, newHeight, d3d11Fmt, 0);
        o_assert(SUCCEEDED(hr));
        this->createDefaultRenderTarget(newWidth, newHeight);
    }
}

} // namespace _priv
} // namespace Oryol
