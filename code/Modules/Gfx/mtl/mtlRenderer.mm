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
mtlRenderer::setup(const GfxSetup& /*setup*/, const gfxPointers& ptrs) {
    o_assert_dbg(!this->valid);
    
    this->valid = true;
    this->pointers = ptrs;

    // sync semaphore
    mtlInflightSemaphore = dispatch_semaphore_create(3);

    // setup central metal objects
    this->device = this->pointers.displayMgr->cocoa.metalDevice;
    this->commandQueue = [this->device newCommandQueue];
    this->commandQueue.label = @"OryolCommandQueue";
}

//------------------------------------------------------------------------------
void
mtlRenderer::discard() {
    o_assert_dbg(this->valid);

    // FIXME

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
mtlRenderer::applyRenderTarget(texture* rt) {
    o_assert_dbg(this->valid);
    if (this->curCommandBuffer == nil) {
        this->curCommandBuffer = [this->commandQueue commandBuffer];
        this->curCommandBuffer.label = @"OryolCommandBuffer";
    }
    if (this->curCommandEncoder != nil) {
        [this->curCommandEncoder endEncoding];
    }
    if (nullptr == rt) {
        // default render target
        this->curDrawable = [this->pointers.displayMgr->cocoa.metalLayer nextDrawable];
        MTLRenderPassDescriptor* passDesc = [MTLRenderPassDescriptor new];
        passDesc.colorAttachments[0].texture = [this->curDrawable texture];
        passDesc.colorAttachments[0].loadAction = MTLLoadActionClear;
static float r = 0.0f;
r += 0.01f;
if (r >= 1.0f) r = 0.0f;
        passDesc.colorAttachments[0].clearColor = MTLClearColorMake(r, 0.0, 0.0, 1.0);
        this->curCommandEncoder = [this->curCommandBuffer renderCommandEncoderWithDescriptor:passDesc];
    }


    Log::Info("mtlRenderer::applyRenderTarget()\n");
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
mtlRenderer::clear(ClearTarget::Mask clearMask, const glm::vec4& color, float32 depth, uint8 stencil) {
    o_assert_dbg(this->valid);
    Log::Info("mtlRenderer::clear()\n");
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
