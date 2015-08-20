//------------------------------------------------------------------------------
//  d3d12DisplayMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12_impl.h"
#include "d3d12DisplayMgr.h"
#include "d3d12Types.h"
#include "Gfx/Core/renderer.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d12DisplayMgr::d3d12DisplayMgr() :
dxgiFactory(nullptr),
d3d12Device(nullptr),
d3d12CommandQueue(nullptr),
dxgiSwapChain(nullptr),
d3d12RTVHeap(nullptr),
rtvDescriptorSize(0),
curBackbufferIndex(0) {
    this->d3d12RenderTargets.Fill(nullptr);
}

//------------------------------------------------------------------------------
d3d12DisplayMgr::~d3d12DisplayMgr() {
    if (this->IsDisplayValid()) {
        this->DiscardDisplay();
    }
}

//------------------------------------------------------------------------------
void
d3d12DisplayMgr::SetupDisplay(const GfxSetup& setup, const gfxPointers& ptrs) {
    o_assert_dbg(!this->IsDisplayValid());
    winDisplayMgr::SetupDisplay(setup, ptrs, " (D3D12)");
    #if ORYOL_DEBUG
    this->enableDebugLayer();
    #endif
    this->createDXGIFactory();
    this->createDeviceObjects();
    this->createSwapChain();
    this->createDefaultRenderTarget(setup.Width, setup.Height);
    this->curBackbufferIndex = this->dxgiSwapChain->GetCurrentBackBufferIndex();
}

//------------------------------------------------------------------------------
void
d3d12DisplayMgr::DiscardDisplay() {
    o_assert_dbg(this->IsDisplayValid());
    
    // NOTE: this method must be called after d3d12Renderer::discard(), since
    // d3d12Renderer::discard() waits until the GPU is finished!
    this->destroyDefaultRenderTarget();
    this->destroySwapChain();
    this->destroyDeviceObjects();
    this->destroyDXGIFactory();
    winDisplayMgr::DiscardDisplay();
}

//------------------------------------------------------------------------------
void
d3d12DisplayMgr::Present() {
    o_assert_dbg(this->dxgiSwapChain);
    this->dxgiSwapChain->Present(this->gfxSetup.SwapInterval, 0);
    this->pointers.renderer->waitForPreviousFrame();
    this->curBackbufferIndex = this->dxgiSwapChain->GetCurrentBackBufferIndex();
}

//------------------------------------------------------------------------------
void
d3d12DisplayMgr::onWindowDidResize() {
    o_warn("d3d12DisplayMgr::onWindowDidResize()\n");
}

//------------------------------------------------------------------------------
void
d3d12DisplayMgr::createDXGIFactory() {
    o_assert_dbg(nullptr == this->dxgiFactory);

    HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&this->dxgiFactory);
    o_assert(SUCCEEDED(hr) && this->dxgiFactory);
}

//------------------------------------------------------------------------------
void
d3d12DisplayMgr::destroyDXGIFactory() {
    if (this->dxgiFactory) {
        this->dxgiFactory->Release();
        this->dxgiFactory = nullptr;
    }
}

//------------------------------------------------------------------------------
void
d3d12DisplayMgr::createDeviceObjects() {
    o_assert_dbg(this->dxgiFactory);
    o_assert_dbg(nullptr == this->d3d12Device);

    HRESULT hr;
    hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)&this->d3d12Device);
    o_assert(SUCCEEDED(hr) && this->d3d12Device);

    D3D12_COMMAND_QUEUE_DESC cmdQueueDesc;
    Memory::Clear(&cmdQueueDesc, sizeof(cmdQueueDesc));
    cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    hr = this->d3d12Device->CreateCommandQueue(&cmdQueueDesc, __uuidof(ID3D12CommandQueue), (void**)&this->d3d12CommandQueue);
    o_assert(SUCCEEDED(hr) && this->d3d12CommandQueue);
}

//------------------------------------------------------------------------------
void
d3d12DisplayMgr::destroyDeviceObjects() {
    if (this->d3d12CommandQueue) {
        this->d3d12CommandQueue->Release();
        this->d3d12CommandQueue = nullptr;
    }
    if (this->d3d12Device) {
        this->d3d12Device->Release();
        this->d3d12Device = nullptr;
    }
}

//------------------------------------------------------------------------------
void
d3d12DisplayMgr::createSwapChain() {
    o_assert_dbg(this->dxgiFactory);
    o_assert_dbg(this->d3d12Device);
    o_assert_dbg(this->d3d12CommandQueue);
    o_assert_dbg(nullptr == this->dxgiSwapChain);
    o_assert_dbg(this->hwnd);

    HRESULT hr;
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    Memory::Clear(&swapChainDesc, sizeof(swapChainDesc));
    swapChainDesc.BufferDesc.Width = this->gfxSetup.Width;
    swapChainDesc.BufferDesc.Height = this->gfxSetup.Height;
    swapChainDesc.BufferDesc.Format = d3d12Types::asSwapChainFormat(this->gfxSetup.ColorFormat);
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.SampleDesc.Count = this->gfxSetup.SampleCount;
    swapChainDesc.SampleDesc.Quality = 0; // this->gfxSetup.SampleCount > 1 ? D3D12_STANDARD_MULTISAMPLE_PATTERN : 0;
    o_warn("FIXME: swapChainDesc.SampleDesc.Quality!\n");
    swapChainDesc.BufferCount = d3d12Config::NumFrames;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = this->hwnd;
    swapChainDesc.Windowed = this->gfxSetup.Windowed;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Flags = 0;
    hr = this->dxgiFactory->CreateSwapChain(this->d3d12CommandQueue, &swapChainDesc, (IDXGISwapChain**)&this->dxgiSwapChain);
    o_assert(SUCCEEDED(hr) && this->dxgiSwapChain);
}

//------------------------------------------------------------------------------
void
d3d12DisplayMgr::destroySwapChain() {
    if (this->dxgiSwapChain) {
        this->dxgiSwapChain->Release();
        this->dxgiSwapChain = nullptr;
    }
}

//------------------------------------------------------------------------------
void
d3d12DisplayMgr::enableDebugLayer() {
    #if ORYOL_DEBUG
    ID3D12Debug* debugController = nullptr;
    HRESULT hr = D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**) &debugController);
    if (SUCCEEDED(hr)) {
        o_assert_dbg(debugController);
        debugController->EnableDebugLayer();
    }
    #endif
}

//------------------------------------------------------------------------------
void
d3d12DisplayMgr::createDefaultRenderTarget(int width, int height) {
    o_assert_dbg(this->d3d12Device);
    o_assert_dbg(nullptr == this->d3d12RTVHeap);
    o_assert_dbg(nullptr == this->d3d12RenderTargets[0]);
    o_assert_dbg((width > 0) && (height > 0));
    HRESULT hr;

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
    Memory::Clear(&heapDesc, sizeof(heapDesc));
    heapDesc.NumDescriptors = d3d12Config::NumFrames;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    hr = this->d3d12Device->CreateDescriptorHeap(&heapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&this->d3d12RTVHeap);
    o_assert(SUCCEEDED(hr) && this->d3d12RTVHeap);
    this->rtvDescriptorSize = this->d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    o_assert(this->rtvDescriptorSize > 0);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = this->d3d12RTVHeap->GetCPUDescriptorHandleForHeapStart();
    for (int i = 0; i < d3d12Config::NumFrames; i++) {
        hr = this->dxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&this->d3d12RenderTargets[i]);
        o_assert(SUCCEEDED(hr) && this->d3d12RenderTargets[i]);
        this->d3d12Device->CreateRenderTargetView(this->d3d12RenderTargets[i], nullptr, rtvHandle);
        rtvHandle.ptr += this->rtvDescriptorSize;
    }

    // FIXME: create depth-stencil buffer!
    o_warn("Create depth-stencil buffer!\n");
}

//------------------------------------------------------------------------------
void
d3d12DisplayMgr::destroyDefaultRenderTarget() {
    o_assert_dbg(this->d3d12RenderTargets[0]);
    o_assert_dbg(this->d3d12RTVHeap);

    // FIXME: destroy depth-stencil buffer!
    o_warn("Destroy depth-stencil buffer!\n");

    for (auto& rt : this->d3d12RenderTargets) {
        if (rt) {
            rt->Release();
            rt = nullptr;
        }
    }
    if (this->d3d12RTVHeap) {
        this->d3d12RTVHeap->Release();
        this->d3d12RTVHeap = nullptr;
    }
}

} // namespace _priv
} // namespace Oryol

