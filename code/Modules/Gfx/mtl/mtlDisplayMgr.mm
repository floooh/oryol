//------------------------------------------------------------------------------
//  mtlDisplayMgr.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlDisplayMgr.h"
#include "mtlTypes.h"
#include "Core/String/StringBuilder.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mtlDisplayMgr::mtlDisplayMgr() :
depthStencilBuffer(nil) {
    // empty
}

//------------------------------------------------------------------------------
mtlDisplayMgr::~mtlDisplayMgr() {
    if (this->IsDisplayValid()) {
        this->DiscardDisplay();
    }
}

//------------------------------------------------------------------------------
void
mtlDisplayMgr::SetupDisplay(const GfxSetup& setup, const gfxPointers& ptrs) {
    o_assert(!this->IsDisplayValid());

    displayMgrBase::SetupDisplay(setup, ptrs);

    this->cocoa.init();
    this->createWindow(setup);
    if (setup.DepthFormat != PixelFormat::None) {
        this->createDepthStencilBuffer(setup.Width, setup.Height);
    }
    if (setup.Samples > 1) {
        this->createMSAABuffer(setup.Width, setup.Height);
    }
}

//------------------------------------------------------------------------------
void
mtlDisplayMgr::DiscardDisplay() {
    o_assert(this->IsDisplayValid());

    this->destroyMSAABuffer();
    this->destroyDepthStencilBuffer();
    this->destroyWindow();
    this->cocoa.terminate();

    displayMgrBase::DiscardDisplay();
}

//------------------------------------------------------------------------------
void
mtlDisplayMgr::Present() {
    Log::Info("mtlDisplayMgr::Present()\n");
}

//------------------------------------------------------------------------------
bool
mtlDisplayMgr::QuitRequested() const {
    return this->cocoa.windowShouldClose();
}

//------------------------------------------------------------------------------
void
mtlDisplayMgr::ProcessSystemEvents() {
    this->cocoa.pollEvents();
}

//------------------------------------------------------------------------------
void
mtlDisplayMgr::createWindow(const GfxSetup& setup) {

    StringBuilder strBuilder(setup.Title);
    strBuilder.Append(" (Metal)");
    this->cocoa.createWindow(setup);
}

//------------------------------------------------------------------------------
void
mtlDisplayMgr::destroyWindow() {
    this->cocoa.destroyWindow();
}

//------------------------------------------------------------------------------
void
mtlDisplayMgr::createDepthStencilBuffer(int width, int height) {
    o_assert_dbg(nil == this->depthStencilBuffer);
    o_assert_dbg(this->gfxSetup.DepthFormat != PixelFormat::None);

    // create depth buffer
    MTLTextureDescriptor* desc = [MTLTextureDescriptor
        texture2DDescriptorWithPixelFormat:mtlTypes::asRenderTargetFormat(this->gfxSetup.DepthFormat)
        width:width height:height mipmapped:NO
    ];
    if (this->gfxSetup.Samples > 1) {
        desc.textureType = MTLTextureType2DMultisample;
        desc.sampleCount = this->gfxSetup.Samples;
    }
    else {
        desc.textureType = MTLTextureType2D;
    }
    this->depthStencilBuffer = [this->cocoa.mtlDevice newTextureWithDescriptor:desc];
}

//------------------------------------------------------------------------------
void
mtlDisplayMgr::destroyDepthStencilBuffer() {
    if (nil != this->depthStencilBuffer) {
        [this->depthStencilBuffer release];
        this->depthStencilBuffer = nil;
    }
}

//------------------------------------------------------------------------------
void
mtlDisplayMgr::createMSAABuffer(int width, int height) {
    o_assert_dbg(nil == this->msaaBuffer);

    o_error("mtlDisplayMgr::createMSAABuffer: FIXME!\n");
}

//------------------------------------------------------------------------------
void
mtlDisplayMgr::destroyMSAABuffer() {
    if (nil != this->msaaBuffer) {
        [this->msaaBuffer release];
        this->msaaBuffer = nil;
    }
}

} // namespace _priv
} // namespace Oryol
