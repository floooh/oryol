//------------------------------------------------------------------------------
//  mtlRenderer.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlRenderer.h"
#include "mtlTypes.h"
#include "Gfx/Core/displayMgr.h"
#include "Gfx/Resource/drawState.h"
#include "Gfx/Resource/mesh.h"

namespace Oryol {
namespace _priv {

dispatch_semaphore_t mtlInflightSemaphore;

//------------------------------------------------------------------------------
mtlRenderer::mtlRenderer() :
valid(false),
rtValid(false),
curDrawState(nullptr),
curDrawable(nil),
mtlDevice(nil),
commandQueue(nil),
curCommandBuffer(nil),
curCommandEncoder(nil) {
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
    this->mtlDevice = this->pointers.displayMgr->cocoa.mtlDevice;
    this->commandQueue = [this->mtlDevice newCommandQueue];
    this->commandQueue.label = @"OryolCommandQueue";
}

//------------------------------------------------------------------------------
void
mtlRenderer::discard() {
    o_assert_dbg(this->valid);

    this->commandQueue = nil;
    this->mtlDevice = nil;
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
    o_assert_dbg(nil != this->curDrawable);

    this->rtValid = false;

    __block dispatch_semaphore_t blockSema = mtlInflightSemaphore;
    [this->curCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
        dispatch_semaphore_signal(blockSema);
    }];

    [this->curCommandEncoder endEncoding];
    [this->curCommandBuffer presentDrawable:curDrawable];
    [this->curCommandBuffer commit];

    // FIXME: probably not a good idea to wait here right after the commit!
    dispatch_semaphore_wait(mtlInflightSemaphore, DISPATCH_TIME_FOREVER);
    this->curCommandEncoder = nil;
    this->curCommandBuffer = nil;
    this->curDrawable = nil;
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
        this->curCommandEncoder = nil;
    }

    // default, or offscreen render target?
    if (nullptr == rt) {
        // default render target
        this->rtAttrs = this->pointers.displayMgr->GetDisplayAttrs();
        this->curDrawable = [this->pointers.displayMgr->cocoa.mtlLayer nextDrawable];
        o_assert(nil != this->curDrawable);
    }
    else {
        o_error("FIXME: mtlRenderer::applyRenderTarget(): offscreen render target!\n");
    }
    this->rtValid = true;

    // init renderpass descriptor
    MTLRenderPassDescriptor* passDesc = [MTLRenderPassDescriptor renderPassDescriptor];

    passDesc.colorAttachments[0].texture = [curDrawable texture];
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
    o_assert_dbg(this->curCommandEncoder);

    if (nullptr == ds) {
        // drawState dependencies still pending, invalidate rendering
        this->curDrawState = nullptr;
    }
    else {
        o_assert_dbg(ds->mtlRenderPipelineState);
        o_assert_dbg(ds->mtlDepthStencilState);
        o_assert2_dbg(ds->Setup.BlendState.ColorFormat == this->rtAttrs.ColorPixelFormat, "ColorFormat in BlendState must match current render target!\n");
        o_assert2_dbg(ds->Setup.BlendState.DepthFormat == this->rtAttrs.DepthPixelFormat, "DepthFormat in BlendSTate must match current render target!\n");
        o_assert2_dbg(ds->Setup.RasterizerState.SampleCount == this->rtAttrs.SampleCount, "SampleCount in RasterizerState must match current render target!\n");

        // set current draw state (used to indicate that rendering is valid)
        this->curDrawState = ds;

        // apply general state
        const glm::vec4& bc = ds->Setup.BlendColor;
        const RasterizerState& rs = ds->Setup.RasterizerState;
        const DepthStencilState& dss = ds->Setup.DepthStencilState;
        [this->curCommandEncoder setBlendColorRed:bc.x green:bc.y blue:bc.z alpha:bc.w];
        [this->curCommandEncoder setCullMode:mtlTypes::asCullMode(rs.CullFaceEnabled, rs.CullFace)];
        [this->curCommandEncoder setStencilReferenceValue:dss.StencilRef];

        // apply state objects
        [this->curCommandEncoder setRenderPipelineState:ds->mtlRenderPipelineState];
        [this->curCommandEncoder setDepthStencilState:ds->mtlDepthStencilState];

        // apply vertex buffers
        const int numVbSlots = ds->meshes.Size();
        for (int vbIndex = 0; vbIndex < numVbSlots; vbIndex++) {
            const mesh* msh = ds->meshes[vbIndex];
            if (msh) {
                o_assert_dbg(msh->mtlVertexBuffer);
                [this->curCommandEncoder setVertexBuffer:msh->mtlVertexBuffer offset:0 atIndex:vbIndex];
            }
            else {
                [this->curCommandEncoder setVertexBuffer:nil offset:0 atIndex:vbIndex];
            }
        }

    }
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
    o_assert2_dbg(this->rtValid, "No render target set!\n");
    o_assert_dbg(this->curCommandEncoder);

    if (nullptr == this->curDrawState) {
        return;
    }
    o_assert_dbg(this->curDrawState->meshes[0]);
    MTLPrimitiveType mtlPrimType = (MTLPrimitiveType)primGroup.PrimType;
    IndexType::Code indexType = this->curDrawState->meshes[0]->indexBufferAttrs.Type;
    if (IndexType::None != indexType) {
        o_assert_dbg(this->curDrawState->meshes[0]->mtlIndexBuffer);
        MTLIndexType mtlIndexType = (MTLIndexType) indexType;
        NSUInteger indexBufferOffset = primGroup.BaseElement * IndexType::ByteSize(indexType);
        [this->curCommandEncoder drawIndexedPrimitives:mtlPrimType
            indexCount:primGroup.NumElements
            indexType:mtlIndexType
            indexBuffer:this->curDrawState->meshes[0]->mtlIndexBuffer
            indexBufferOffset:indexBufferOffset ];
    }
    else {
        [this->curCommandEncoder drawPrimitives:mtlPrimType vertexStart:primGroup.BaseElement vertexCount:primGroup.NumElements];
    }
}

//------------------------------------------------------------------------------
void
mtlRenderer::draw(int32 primGroupIndex) {
    o_assert_dbg(this->valid);
    if (nullptr == this->curDrawState) {
        return;
    }
    o_assert_dbg(this->curDrawState->meshes[0]);
    if (primGroupIndex >= this->curDrawState->meshes[0]->numPrimGroups) {
        // this may happen if rendering a placeholder which doesn't have
        // as many materials as the original mesh
        return;
    }
    const PrimitiveGroup& primGroup = this->curDrawState->meshes[0]->primGroups[primGroupIndex];
    this->draw(primGroup);
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

//------------------------------------------------------------------------------
void
mtlRenderer::invalidateMeshState() {
    // FIXME!
    Log::Info("mtlRenderer::invalidateMeshState()\n");
}

//------------------------------------------------------------------------------
void
mtlRenderer::invalidateProgramState() {
    // FIXME!
    Log::Info("mtlRenderer::invalidateProgramState()\n");
}

//------------------------------------------------------------------------------
void
mtlRenderer::invalidateDrawState() {
    // FIXME!
    Log::Info("mtlRenderer::invalidateDrawState()\n");
}

} // namespace _priv
} // namespace Oryol
