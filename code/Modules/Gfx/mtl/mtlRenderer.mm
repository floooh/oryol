//------------------------------------------------------------------------------
//  mtlRenderer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlRenderer.h"
#include "Gfx/Core/displayMgr.h"

namespace Oryol {
namespace _priv {

dispatch_semaphore_t mtlInflightSemaphore;

//------------------------------------------------------------------------------
mtlRenderer::mtlRenderer() :
valid(false) {
    // FIXME
}

//------------------------------------------------------------------------------
mtlRenderer::~mtlRenderer() {
    o_assert_dbg(!this->valid);
}

//------------------------------------------------------------------------------
void
mtlRenderer::setup(const GfxSetup& setup, const gfxPointers& ptrs) {
    o_assert_dbg(!this->valid);
    
    this->valid = true;
    this->pointers = ptrs;
    this->gfxSetup = setup;

    // sync semaphore
    mtlInflightSemaphore = dispatch_semaphore_create(3);

    // setup central metal objects
    this->device = this->pointers.displayMgr->cocoa.mtlDevice;
    this->commandQueue = [this->device newCommandQueue];
    this->commandQueue.label = @"OryolCommandQueue";
}

//------------------------------------------------------------------------------
void
mtlRenderer::discard() {
    o_assert_dbg(this->valid);

    this->commandQueue = nil;
    this->device = nil;
    this->pointers = gfxPointers();
    this->valid = false;
}

//------------------------------------------------------------------------------
bool
mtlRenderer::isValid() const {
    return this->valid;
}

//------------------------------------------------------------------------------
void
mtlRenderer::resetStateCache() {
    o_assert_dbg(this->valid);

    o_error("mtlRenderer::resetStateCache()\n");
}

//------------------------------------------------------------------------------
bool
mtlRenderer::queryFeature(GfxFeature::Code feat) const {
    o_assert_dbg(this->valid);

    o_error("mtlRenderer::queryFeature()\n");
    return false;
}

//------------------------------------------------------------------------------
void
mtlRenderer::commitFrame() {
    o_assert_dbg(this->valid);
    o_assert_dbg(nil != this->curCommandBuffer);
    __block dispatch_semaphore_t blockSema = mtlInflightSemaphore;
    [this->curCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
        dispatch_semaphore_signal(blockSema);
    }];

    [this->curCommandEncoder endEncoding];
    [this->curCommandBuffer presentDrawable:this->curDrawable];
    [this->curCommandBuffer commit];

    // FIXME: probably not a good idea to wait here right after the commit!
    dispatch_semaphore_wait(mtlInflightSemaphore, DISPATCH_TIME_FOREVER);
    this->curCommandEncoder = nil;
    this->curCommandBuffer = nil;
}

//------------------------------------------------------------------------------
const DisplayAttrs&
mtlRenderer::renderTargetAttrs() const {
    o_assert_dbg(this->valid);

    o_error("mtlRenderer::renderTargetAttrs()\n");

    static DisplayAttrs dummy;
    return dummy;
}

//------------------------------------------------------------------------------
void
mtlRenderer::applyViewPort(int32 x, int32 y, int32 width, int32 height, bool originTopLeft) {
    o_assert_dbg(this->valid);

    o_error("mtlRenderer::applyViewPort()\n");
}

//------------------------------------------------------------------------------
void
mtlRenderer::applyScissorRect(int32 x, int32 y, int32 width, int32 height, bool originTopLeft) {
    o_assert_dbg(this->valid);

    o_error("mtlRenderer::applyScissorRect()\n");
}

//------------------------------------------------------------------------------
void
mtlRenderer::applyRenderTarget(texture* rt, const ClearState& clearState) {
    o_assert_dbg(this->valid);

    // create command buffer if this is the first call in the current frame
    if (this->curCommandBuffer == nil) {
        this->curCommandBuffer = [this->commandQueue commandBuffer];
        this->curCommandBuffer.label = @"OryolCommandBuffer";
    }

    // finish previous command encoder (from previous render pass)
    if (this->curCommandEncoder != nil) {
        [this->curCommandEncoder endEncoding];
    }

    // default, or offscreen render target?
    if (nullptr == rt) {
        // default render target
        this->curDrawable = [this->pointers.displayMgr->cocoa.mtlLayer nextDrawable];
    }
    else {
        o_error("FIXME: mtlRenderer::applyRenderTarget(): offscreen render target!\n");
    }

    // init renderpass descriptor
    MTLRenderPassDescriptor* passDesc = [MTLRenderPassDescriptor renderPassDescriptor];

    passDesc.colorAttachments[0].texture = [this->curDrawable texture];
    if (clearState.Actions & ClearState::ClearColor) {
        passDesc.colorAttachments[0].loadAction = MTLLoadActionClear;
        const glm::vec4& c = clearState.Color;
        passDesc.colorAttachments[0].clearColor = MTLClearColorMake(c.x, c.y, c.z, c.w);
    }
    else {
        passDesc.colorAttachments[0].loadAction = MTLLoadActionDontCare;
    }

    if (nil != this->pointers.displayMgr->depthStencilBuffer) {
        passDesc.depthAttachment.texture = this->pointers.displayMgr->depthStencilBuffer;
        if (clearState.Actions & ClearState::ClearDepth) {
            passDesc.depthAttachment.loadAction = MTLLoadActionClear;
            passDesc.depthAttachment.clearDepth = clearState.Depth;
        }
        else {
            passDesc.depthAttachment.loadAction = MTLLoadActionDontCare;
        }
    }

    if (PixelFormat::IsDepthStencilFormat(this->gfxSetup.DepthFormat)) {
        passDesc.stencilAttachment.texture = this->pointers.displayMgr->depthStencilBuffer;
        if (clearState.Actions & ClearState::ClearStencil) {
            passDesc.stencilAttachment.loadAction = MTLLoadActionClear;
            passDesc.stencilAttachment.clearStencil = clearState.Stencil;
        }
        else {
            passDesc.stencilAttachment.loadAction = MTLLoadActionDontCare;
        }
    }

    // create command encoder for this render pass
    this->curCommandEncoder = [this->curCommandBuffer renderCommandEncoderWithDescriptor:passDesc];
}

//------------------------------------------------------------------------------
void
mtlRenderer::applyDrawState(drawState* ds) {
    o_assert_dbg(this->valid);

    o_error("mtlRenderer::applyDrawState()\n");
}

//------------------------------------------------------------------------------
void
mtlRenderer::applyUniformBlock(int32 blockIndex, int64 layoutHash, const uint8* ptr, int32 byteSize) {
    o_assert_dbg(this->valid);

    o_error("mtlRenderer::applyUniformBlock()\n");
}

//------------------------------------------------------------------------------
void
mtlRenderer::draw(const PrimitiveGroup& primGroup) {
    o_assert_dbg(this->valid);

    o_error("mtlRenderer::draw()\n");
}

//------------------------------------------------------------------------------
void
mtlRenderer::draw(int32 primGroupIndex) {
    o_assert_dbg(this->valid);

    o_error("mtlRenderer::draw()\n");
}

//------------------------------------------------------------------------------
void
mtlRenderer::drawInstanced(const PrimitiveGroup& primGroup, int32 numInstances) {
    o_assert_dbg(this->valid);

    o_error("mtlRenderer::drawInstanced()\n");
}

//------------------------------------------------------------------------------
void
mtlRenderer::drawInstanced(int32 primGroupIndex, int32 numInstances) {
    o_assert_dbg(this->valid);

    o_error("mtlRenderer::drawInstanced()\n");
}

//------------------------------------------------------------------------------
void
mtlRenderer::updateVertices(mesh* msh, const void* data, int32 numBytes) {
    o_assert_dbg(this->valid);

    o_error("mtlRenderer::updateVertices()\n");
}

//------------------------------------------------------------------------------
void
mtlRenderer::readPixels(void* buf, int32 bufNumBytes) {
    o_assert_dbg(this->valid);

    o_error("mtlRenderer::readPixels()\n");
}

} // namespace _priv
} // namespace Oryol
