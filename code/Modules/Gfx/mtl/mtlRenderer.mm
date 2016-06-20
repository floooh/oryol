//------------------------------------------------------------------------------
//  mtlRenderer.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlRenderer.h"
#include "mtlTypes.h"
#include "Gfx/Core/displayMgr.h"
#include "Gfx/Resource/resourcePools.h"
#include "Gfx/Core/UniformBlockLayout.h"
#include "Gfx/Resource/pipeline.h"
#include "Gfx/Resource/mesh.h"
#include "Gfx/Resource/shader.h"
#include "Gfx/Resource/texture.h"

namespace Oryol {
namespace _priv {

dispatch_semaphore_t mtlInflightSemaphore;

//------------------------------------------------------------------------------
mtlRenderer::mtlRenderer() :
valid(false),
frameIndex(0),
curFrameRotateIndex(0),
rtValid(false),
curPipeline(nullptr),
curPrimaryMesh(nullptr),
curMTLPrimitiveType(MTLPrimitiveTypeTriangle),
curMTLIndexType(MTLIndexTypeUInt16),
mtlDevice(nil),
commandQueue(nil),
curCommandBuffer(nil),
curCommandEncoder(nil),
curUniformBufferPtr(nullptr),
curUniformBufferOffset(0) {
    // empty
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
    this->releaseQueue.setup();

    // frame-sync semaphore
    mtlInflightSemaphore = dispatch_semaphore_create(GfxConfig::MtlMaxInflightFrames);

    // setup central metal objects
    this->mtlDevice = osBridge::ptr()->mtlDevice;
    this->commandQueue = [this->mtlDevice newCommandQueue];

    // create global rotated uniform buffers
    for (int i = 0; i < GfxConfig::MtlMaxInflightFrames; i++) {
        this->uniformBuffers[i] = [this->mtlDevice
            newBufferWithLength:setup.GlobalUniformBufferSize
            options:mtlTypes::asBufferResourceOptions(Usage::Stream)];
    }
    this->curFrameRotateIndex = 0;
    this->curUniformBufferOffset = 0;
}

//------------------------------------------------------------------------------
void
mtlRenderer::discard() {
    o_assert_dbg(this->valid);

    // wait for the final frame to finish
    for (int i = 0; i < GfxConfig::MtlMaxInflightFrames; i++) {
        dispatch_semaphore_wait(mtlInflightSemaphore, DISPATCH_TIME_FOREVER);
    }
    for (int i = 0; i < GfxConfig::MtlMaxInflightFrames; i++) {
        this->uniformBuffers[i] = nil;
    }
    this->releaseQueue.discard();
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
    o_warn("mtlRenderer::resetStateCache()\n");
}

//------------------------------------------------------------------------------
bool
mtlRenderer::queryFeature(GfxFeature::Code feat) const {
    switch(feat) {
        #if ORYOL_MACOS
        case GfxFeature::TextureCompressionDXT:
        #else
        case GfxFeature::TextureCompressionPVRTC:
        #endif
        case GfxFeature::TextureFloat:
        case GfxFeature::Instancing:
        case GfxFeature::OriginTopLeft:
            return true;
        default:
            return false;
    }
}

//------------------------------------------------------------------------------
void
mtlRenderer::commitFrame() {
    o_assert_dbg(this->valid);
    o_assert_dbg(nil != this->curCommandBuffer);

    this->rtValid = false;

    // commit the global uniform buffer updates
    #if ORYOL_MACOS
    [this->uniformBuffers[this->curFrameRotateIndex] didModifyRange:NSMakeRange(0, this->curUniformBufferOffset)];
    #endif

    if (nil != this->curCommandEncoder) {
        [this->curCommandEncoder endEncoding];
        [this->curCommandBuffer presentDrawable:[osBridge::ptr()->mtkView currentDrawable]];
    }
    __block dispatch_semaphore_t blockSema = mtlInflightSemaphore;
    [this->curCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
        dispatch_semaphore_signal(blockSema);
    }];
    [this->curCommandBuffer commit];

    // rotate to next uniform buffer
    if (++this->curFrameRotateIndex >= GfxConfig::MtlMaxInflightFrames) {
        this->curFrameRotateIndex = 0;
    }

    this->frameIndex++;
    this->curUniformBufferOffset = 0;
    this->curCommandEncoder = nil;
    this->curCommandBuffer = nil;
    this->curUniformBufferPtr = nullptr;
    this->curPipeline = nullptr;
    this->curPrimaryMesh = nullptr;
}

//------------------------------------------------------------------------------
const DisplayAttrs&
mtlRenderer::renderTargetAttrs() const {
    o_assert_dbg(this->valid);
    return this->rtAttrs;
}

//------------------------------------------------------------------------------
void
mtlRenderer::applyViewPort(int x, int y, int width, int height, bool originTopLeft) {
    o_assert_dbg(this->valid);
    if (nil == this->curCommandEncoder) {
        return;
    }

    MTLViewport vp;
    vp.originX = (double) x;
    vp.originY = (double) (originTopLeft ? y : (this->rtAttrs.FramebufferHeight - (y + height)));
    vp.width   = (double) width;
    vp.height  = (double) height;
    vp.znear   = 0.0;
    vp.zfar    = 1.0;
    [this->curCommandEncoder setViewport:vp];
}

//------------------------------------------------------------------------------
void
mtlRenderer::applyScissorRect(int x, int y, int width, int height, bool originTopLeft) {
    o_assert_dbg(this->valid);
    o_assert_dbg(width >= 0);
    o_assert_dbg(height >= 0);

    if (nil == this->curCommandEncoder) {
        return;
    }

    // clip against frame buffer size
    x = std::min(std::max(0, x), this->rtAttrs.FramebufferWidth - 1);
    y = std::min(std::max(0, y), this->rtAttrs.FramebufferHeight - 1);
    if ((x + width) > this->rtAttrs.FramebufferWidth) {
        width = this->rtAttrs.FramebufferWidth - x;
    }
    if ((y + height) > this->rtAttrs.FramebufferHeight) {
        height = this->rtAttrs.FramebufferHeight - y;
    }
    if (width <= 0) {
        width = 1;
    }
    if (height <= 0) {
        height = 1;
    }

    MTLScissorRect rect;
    rect.x = x;
    rect.y = originTopLeft ? y : this->rtAttrs.FramebufferHeight - (y + height);
    rect.width = width;
    rect.height = height;

    // need to clip against render target
    [this->curCommandEncoder setScissorRect:rect];
}

//------------------------------------------------------------------------------
void
mtlRenderer::applyRenderTarget(texture* rt, const ClearState& clearState) {
    o_assert_dbg(this->valid);

    // create command buffer if this is the first call in the current frame
    if (this->curCommandBuffer == nil) {
        // block until the oldest frame in flight has finished
        dispatch_semaphore_wait(mtlInflightSemaphore, DISPATCH_TIME_FOREVER);
        // safely destroy released GPU resources
        this->releaseQueue.garbageCollect(this->frameIndex);
        // get a new command buffer
        this->curCommandBuffer = [this->commandQueue commandBufferWithUnretainedReferences];
    }

    // finish previous command encoder (from previous render pass)
    if (this->curCommandEncoder != nil) {
        [this->curCommandEncoder endEncoding];
        this->curCommandEncoder = nil;
    }

    // default, or offscreen render target?
    MTLRenderPassDescriptor* passDesc = nil;
    if (nullptr == rt) {
        // default render target
        passDesc = [osBridge::ptr()->mtkView currentRenderPassDescriptor];
        this->rtAttrs = this->pointers.displayMgr->GetDisplayAttrs();
    }
    else {
        passDesc = [MTLRenderPassDescriptor renderPassDescriptor];
        this->rtAttrs = DisplayAttrs::FromTextureAttrs(rt->textureAttrs);
    }
    if (passDesc) {
        this->rtValid = true;
    }
    else {
        // pass descriptor will not be valid if window is minimized
        return;
    }

    // init renderpass descriptor
    if (rt) {
        passDesc.colorAttachments[0].texture = rt->mtlTextures[0];
    }
    if (clearState.Actions & ClearState::ColorBit) {
        passDesc.colorAttachments[0].loadAction = MTLLoadActionClear;
        const glm::vec4& c = clearState.Color;
        passDesc.colorAttachments[0].clearColor = MTLClearColorMake(c.x, c.y, c.z, c.w);
    }
    else {
        passDesc.colorAttachments[0].loadAction = MTLLoadActionDontCare;
    }

    if (PixelFormat::IsDepthFormat(this->rtAttrs.DepthPixelFormat)) {
        if (rt) {
            passDesc.depthAttachment.texture = rt->mtlDepthTex;
        }
        if (clearState.Actions & ClearState::DepthBit) {
            passDesc.depthAttachment.loadAction = MTLLoadActionClear;
            passDesc.depthAttachment.clearDepth = clearState.Depth;
        }
        else {
            passDesc.depthAttachment.loadAction = MTLLoadActionDontCare;
        }
    }

    if (PixelFormat::IsDepthStencilFormat(this->rtAttrs.DepthPixelFormat)) {
        if (rt) {
            passDesc.stencilAttachment.texture = rt->mtlDepthTex;
        }
        if (clearState.Actions & ClearState::StencilBit) {
            passDesc.stencilAttachment.loadAction = MTLLoadActionClear;
            passDesc.stencilAttachment.clearStencil = clearState.Stencil;
        }
        else {
            passDesc.stencilAttachment.loadAction = MTLLoadActionDontCare;
        }
    }

    // create command encoder for this render pass
    // this might return nil if the window is minimized
    this->curCommandEncoder = [this->curCommandBuffer renderCommandEncoderWithDescriptor:passDesc];

    // bind uniform buffer to shader stages, only needs to happen once
    if (nil != this->curCommandEncoder) {
        for (int bindSlot = 0; bindSlot < GfxConfig::MaxNumUniformBlocksPerStage; bindSlot++) {
            [this->curCommandEncoder
                setVertexBuffer:this->uniformBuffers[this->curFrameRotateIndex]
                offset:0
                atIndex:bindSlot];
            [this->curCommandEncoder
                setFragmentBuffer:this->uniformBuffers[this->curFrameRotateIndex]
                offset:0
                atIndex:bindSlot];
        }
    }

    // get the base pointer for the uniform buffer, this only happens once per frame
    if (nullptr == this->curUniformBufferPtr) {
        this->curUniformBufferPtr = (uint8*)[this->uniformBuffers[this->curFrameRotateIndex] contents];
    }
}

//------------------------------------------------------------------------------
void
mtlRenderer::applyDrawState(pipeline* pip, mesh** meshes, int numMeshes) {
    o_assert_dbg(this->valid);
    o_assert_dbg(pip);
    o_assert_dbg(meshes && (numMeshes > 0));

    if (nil == this->curCommandEncoder) {
        return;
    }
    // if any of the meshes are still loading, cancel the next draw-call
    for (int i = 0; i < numMeshes; i++) {
        if (nullptr == meshes[i]) {
            this->curPipeline = nullptr;
            return;
        }
    }
    o_assert_dbg(pip->mtlRenderPipelineState);
    o_assert_dbg(pip->mtlDepthStencilState);
    o_assert2_dbg(pip->Setup.BlendState.ColorFormat == this->rtAttrs.ColorPixelFormat, "ColorFormat in BlendState must match current render target!\n");
    o_assert2_dbg(pip->Setup.BlendState.DepthFormat == this->rtAttrs.DepthPixelFormat, "DepthFormat in BlendSTate must match current render target!\n");
    o_assert2_dbg(pip->Setup.RasterizerState.SampleCount == this->rtAttrs.SampleCount, "SampleCount in RasterizerState must match current render target!\n");

    // need to store draw state and first mesh for later draw call
    this->curPipeline = pip;
    this->curPrimaryMesh = meshes[0];
    o_assert_dbg(this->curPrimaryMesh);

    // apply general state
    const glm::vec4& bc = pip->Setup.BlendColor;
    const RasterizerState& rs = pip->Setup.RasterizerState;
    const DepthStencilState& dss = pip->Setup.DepthStencilState;
    [this->curCommandEncoder setBlendColorRed:bc.x green:bc.y blue:bc.z alpha:bc.w];
    [this->curCommandEncoder setCullMode:mtlTypes::asCullMode(rs.CullFaceEnabled, rs.CullFace)];
    [this->curCommandEncoder setStencilReferenceValue:dss.StencilRef];

    // apply state objects
    [this->curCommandEncoder setRenderPipelineState:pip->mtlRenderPipelineState];
    [this->curCommandEncoder setDepthStencilState:pip->mtlDepthStencilState];

    // apply vertex buffers
    for (int meshIndex = 0; meshIndex < GfxConfig::MaxNumInputMeshes; meshIndex++) {
        const mesh* msh = meshIndex < numMeshes ? meshes[meshIndex] : nullptr;
        // NOTE: vertex buffers are located after constant buffers
        const int vbSlotIndex = meshIndex + GfxConfig::MaxNumUniformBlocksPerStage;
        if (msh) {
            // note: vb.mtlBuffers[vb.activeSlot] can be nil!
            const auto& vb = msh->buffers[mesh::vb];
            [this->curCommandEncoder setVertexBuffer:vb.mtlBuffers[vb.activeSlot] offset:0 atIndex:vbSlotIndex];
        }
        else {
            [this->curCommandEncoder setVertexBuffer:nil offset:0 atIndex:vbSlotIndex];
        }
    }

    // store additional state for following draw calls
    this->curMTLPrimitiveType = mtlTypes::asPrimitiveType(pip->Setup.PrimType);
    if (this->curPrimaryMesh->indexBufferAttrs.Type != IndexType::None) {
        this->curMTLIndexType = mtlTypes::asIndexType(this->curPrimaryMesh->indexBufferAttrs.Type);
    }
}

//------------------------------------------------------------------------------
void
mtlRenderer::applyUniformBlock(ShaderStage::Code bindStage, int bindSlot, int64_t layoutHash, const uint8_t* ptr, int byteSize) {
    o_assert_dbg(this->valid);
    if (nil == this->curCommandEncoder) {
        return;
    }
    if (nullptr == this->curPipeline) {
        return;
    }

    #if ORYOL_DEBUG
    // check whether the provided struct is type-compatible with the uniform layout
    shader* shd = this->curPipeline->shd;
    o_assert_dbg(shd);
    int ubIndex = shd->Setup.UniformBlockIndexByStageAndSlot(bindStage, bindSlot);
    const UniformBlockLayout& layout = shd->Setup.UniformBlockLayout(ubIndex);
    o_assert2_dbg(layout.TypeHash == layoutHash, "incompatible uniform block!\n");
    o_assert_dbg(byteSize == layout.ByteSize());
    o_assert2_dbg((this->curUniformBufferOffset + byteSize) <= this->gfxSetup.GlobalUniformBufferSize, "Global uniform buffer exhausted!\n");
    o_assert_dbg((this->curUniformBufferOffset & (MtlUniformAlignment-1)) == 0);
    #endif

    // write uniforms into global uniform buffer, advance buffer offset
    // and set current uniform buffer location on command-encoder
    // NOTE: we'll call didModifyRange only ONCE inside commitFrame!
    uint8* dstPtr = this->curUniformBufferPtr + this->curUniformBufferOffset;
    std::memcpy(dstPtr, ptr, byteSize);
    if (ShaderStage::VS == bindStage) {
        [this->curCommandEncoder setVertexBufferOffset:this->curUniformBufferOffset atIndex:bindSlot];
    }
    else {
        [this->curCommandEncoder setFragmentBufferOffset:this->curUniformBufferOffset atIndex:bindSlot];
    }
    this->curUniformBufferOffset = Memory::RoundUp(this->curUniformBufferOffset + byteSize, MtlUniformAlignment);
}

//------------------------------------------------------------------------------
void
mtlRenderer::applyTextures(ShaderStage::Code bindStage, texture** textures, int numTextures) {
    o_assert_dbg(this->valid);
    o_assert_dbg(((ShaderStage::VS == bindStage) && (numTextures <= GfxConfig::MaxNumVertexTextures)) ||
                 ((ShaderStage::FS == bindStage) && (numTextures <= GfxConfig::MaxNumFragmentTextures)));
    if (nil == this->curCommandEncoder) {
        return;
    }
    if (nullptr == this->curPipeline) {
        return;
    }

    // if any of the texture pointers is null, this means the texture hasn't loaded
    // yet or has failed loading, in this case, disable the next draw call
    for (int i = 0; i < numTextures; i++) {
        if (nullptr == textures[i]) {
            this->curPipeline = nullptr;
            return;
        }
    }

    // apply textures and samplers
    if (ShaderStage::VS == bindStage) {
        for (int i = 0; i < numTextures; i++) {
            const texture* tex = textures[i];
            [this->curCommandEncoder setVertexTexture:tex->mtlTextures[tex->activeSlot] atIndex:i];
            [this->curCommandEncoder setVertexSamplerState:tex->mtlSamplerState atIndex:i];
        }
    }
    else {
        for (int i = 0; i < numTextures; i++) {
            const texture* tex = textures[i];
            [this->curCommandEncoder setFragmentTexture:tex->mtlTextures[tex->activeSlot] atIndex:i];
            [this->curCommandEncoder setFragmentSamplerState:tex->mtlSamplerState atIndex:i];
        }
    }
}

//------------------------------------------------------------------------------
void
mtlRenderer::drawInstanced(const PrimitiveGroup& primGroup, int numInstances) {
    o_assert_dbg(this->valid);
    if (nil == this->curCommandEncoder) {
        return;
    }
    if (nullptr == this->curPipeline) {
        return;
    }
    const mesh* msh = this->curPrimaryMesh;
    o_assert_dbg(msh);
    if (IndexType::None == msh->indexBufferAttrs.Type) {
        [this->curCommandEncoder drawPrimitives:(MTLPrimitiveType)this->curMTLPrimitiveType
            vertexStart:primGroup.BaseElement
            vertexCount:primGroup.NumElements
            instanceCount:numInstances];
    }
    else {
        const auto& ib = msh->buffers[mesh::ib];
        o_assert_dbg(nil != ib.mtlBuffers[ib.activeSlot]);
        NSUInteger indexBufferOffset = primGroup.BaseElement * IndexType::ByteSize(msh->indexBufferAttrs.Type);
        [this->curCommandEncoder drawIndexedPrimitives:(MTLPrimitiveType)this->curMTLPrimitiveType
            indexCount:primGroup.NumElements
            indexType:(MTLIndexType)this->curMTLIndexType
            indexBuffer:ib.mtlBuffers[ib.activeSlot]
            indexBufferOffset:indexBufferOffset
            instanceCount:numInstances ];
    }
}

//------------------------------------------------------------------------------
void
mtlRenderer::drawInstanced(int primGroupIndex, int numInstances) {
    if (nil == this->curCommandEncoder) {
        return;
    }
    if (nullptr == this->curPipeline) {
        return;
    }
    const mesh* msh = this->curPrimaryMesh;
    o_assert_dbg(msh);
    if (primGroupIndex >= msh->numPrimGroups) {
        // this may happen if rendering a placeholder which doesn't have
        // as many materials as the original mesh
        return;
    }
    const PrimitiveGroup& primGroup = msh->primGroups[primGroupIndex];
    this->drawInstanced(primGroup, numInstances);
}

//------------------------------------------------------------------------------
void
mtlRenderer::draw(const PrimitiveGroup& primGroup) {
    this->drawInstanced(primGroup, 1);
}

//------------------------------------------------------------------------------
void
mtlRenderer::draw(int primGroupIndex) {
    this->drawInstanced(primGroupIndex, 1);
}

//------------------------------------------------------------------------------
void
meshBufferRotateActiveSlot(mesh::buffer& buf, int frameIndex) {
    // helper function to get the right double-buffered
    // vertex or index buffer for a buffer update
    
    // restrict buffer updates to once per frame per mesh, this isn't
    // strictly required on GL, but we want the same restrictions across all 3D APIs
    o_assert2(buf.updateFrameIndex != frameIndex, "Only one data update allowed per buffer and frame!\n");
    buf.updateFrameIndex = frameIndex;

    // if usage is streaming, rotate slot index to next dynamic vertex buffer
    // to implement double/multi-buffering because the previous buffer
    // might still be in-flight on the GPU
    o_assert_dbg(buf.numSlots > 1);
    if (++buf.activeSlot >= buf.numSlots) {
        buf.activeSlot = 0;
    }
}

//------------------------------------------------------------------------------
void
mtlRenderer::updateVertices(mesh* msh, const void* data, int numBytes) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != msh);
    o_assert_dbg(nullptr != data);
    o_assert_dbg((numBytes > 0) && (numBytes <= msh->vertexBufferAttrs.ByteSize()));
    o_assert_dbg(Usage::Immutable != msh->vertexBufferAttrs.BufferUsage);

    auto& vb = msh->buffers[mesh::vb];
    meshBufferRotateActiveSlot(vb, this->frameIndex);
    o_assert_dbg(nil != vb.mtlBuffers[vb.activeSlot]);
    o_assert_dbg(numBytes <= int([vb.mtlBuffers[vb.activeSlot] length]));
    void* dstPtr = [vb.mtlBuffers[vb.activeSlot] contents];
    std::memcpy(dstPtr, data, numBytes);
    #if ORYOL_MACOS
    [vb.mtlBuffers[vb.activeSlot] didModifyRange:NSMakeRange(0, numBytes)];
    #endif
}

//------------------------------------------------------------------------------
void
mtlRenderer::updateIndices(mesh* msh, const void* data, int numBytes) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != msh);
    o_assert_dbg(nullptr != data);
    o_assert_dbg((numBytes > 0) && (numBytes <= msh->indexBufferAttrs.ByteSize()));
    o_assert_dbg(Usage::Immutable != msh->indexBufferAttrs.BufferUsage);

    auto& ib = msh->buffers[mesh::ib];
    meshBufferRotateActiveSlot(ib, this->frameIndex);
    o_assert_dbg(nil != ib.mtlBuffers[ib.activeSlot]);
    o_assert_dbg(numBytes <= int([ib.mtlBuffers[ib.activeSlot] length]));
    void* dstPtr = [ib.mtlBuffers[ib.activeSlot] contents];
    std::memcpy(dstPtr, data, numBytes);
    #if ORYOL_MACOS
    [ib.mtlBuffers[ib.activeSlot] didModifyRange:NSMakeRange(0, numBytes)];
    #endif
}

//------------------------------------------------------------------------------
void
texRotateActiveSlot(texture* tex, int frameIndex) {
    o_assert2(tex->updateFrameIndex != frameIndex, "Only one data update allowed per texture and frame!\n");
    tex->updateFrameIndex = frameIndex;
    o_assert_dbg(tex->numSlots > 1);
    if (++tex->activeSlot >= tex->numSlots) {
        tex->activeSlot = 0;
    }
}

//------------------------------------------------------------------------------
void
mtlRenderer::updateTexture(texture* tex, const void* data, const ImageDataAttrs& offsetsAndSizes) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nullptr != tex);
    o_assert_dbg(nullptr != data);

    const TextureAttrs& attrs = tex->textureAttrs;
    o_assert_dbg(TextureType::Texture2D == attrs.Type);
    o_assert_dbg(Usage::Immutable != attrs.TextureUsage);
    o_assert_dbg(!PixelFormat::IsCompressedFormat(attrs.ColorFormat));
    o_assert_dbg(offsetsAndSizes.NumMipMaps == attrs.NumMipMaps);
    o_assert_dbg(offsetsAndSizes.NumFaces == 1);

    texRotateActiveSlot(tex, this->frameIndex);
    o_assert_dbg(nil != tex->mtlTextures[tex->activeSlot]);

    // copy data bytes into texture
    const uint8* srcPtr = (const uint8*) data;
    for (int mipIndex = 0; mipIndex < attrs.NumMipMaps; mipIndex++) {
        int mipWidth = std::max(attrs.Width >> mipIndex, 1);
        int mipHeight = std::max(attrs.Height >> mipIndex, 1);
        // special case PVRTC formats: bytesPerRow must be 0
        int bytesPerRow = PixelFormat::RowPitch(attrs.ColorFormat, mipWidth);
        MTLRegion region = MTLRegionMake2D(0, 0, mipWidth, mipHeight);
        [tex->mtlTextures[tex->activeSlot] replaceRegion:region
            mipmapLevel:mipIndex
            slice:0
            withBytes:srcPtr+offsetsAndSizes.Offsets[0][mipIndex]
            bytesPerRow:bytesPerRow
            bytesPerImage:0];
    }
}

//------------------------------------------------------------------------------
void
mtlRenderer::readPixels(void* buf, int bufNumBytes) {
    o_assert_dbg(this->valid);
    o_warn("mtlRenderer::readPixels()\n");
}

//------------------------------------------------------------------------------
void
mtlRenderer::releaseDeferred(ORYOL_OBJC_ID obj) {
    o_assert_dbg(nil != obj);
    this->releaseQueue.releaseDeferred(this->frameIndex, obj);
}

} // namespace _priv
} // namespace Oryol
