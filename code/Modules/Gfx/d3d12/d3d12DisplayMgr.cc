//------------------------------------------------------------------------------
//  d3d12DisplayMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12_impl.h"
#include "d3d12DisplayMgr.h"

using namespace Microsoft::WRL;

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d12DisplayMgr::d3d12DisplayMgr() {
    // FIXME
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
    this->createDeviceAndSwapChain();
}

//------------------------------------------------------------------------------
void
d3d12DisplayMgr::DiscardDisplay() {
    o_assert_dbg(this->IsDisplayValid());
    winDisplayMgr::DiscardDisplay();
}

//------------------------------------------------------------------------------
void
d3d12DisplayMgr::Present() {
    o_warn("d3d12DisplayMgr::Present()\n");
}

//------------------------------------------------------------------------------
void
d3d12DisplayMgr::onWindowDidResize() {
    o_warn("d3d12DisplayMgr::onWindowDidResize()\n");
}

//------------------------------------------------------------------------------
void
d3d12DisplayMgr::createDeviceAndSwapChain() {
    

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

} // namespace _priv
} // namespace Oryol

