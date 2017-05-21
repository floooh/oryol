//------------------------------------------------------------------------------
//  mtlRenderer.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlRenderer.h"
#include "mtlTypes.h"
#include "Gfx/private/displayMgr.h"
#include "Gfx/private/resourcePools.h"
#include "Gfx/private/resource.h"
#include "Gfx/GfxTypes.h"

namespace Oryol {
namespace _priv {

dispatch_semaphore_t mtlInflightSemaphore;

//------------------------------------------------------------------------------
mtlRenderer::mtlRenderer() :
valid(false),
frameIndex(0),
curFrameRotateIndex(0),
rpValid(false),
curPipeline(nullptr),
curPrimaryMesh(nullptr),
curMTLPrimitiveType(MTLPrimitiveTypeTriangle),
curMTLIndexType(MTLIndexTypeUInt16),
mtlDevice(nil),
commandQueue(nil),
curCommandBuffer(nil),
curRenderCmdEncoder(nil),
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

    // frame-sync semaphore
    mtlInflightSemaphore = dispatch_semaphore_create(GfxConfig::MaxInflightFrames);

    // setup central metal objects
    this->mtlDevice = osBridge::ptr()->mtlDevice;
    this->commandQueue = [this->mtlDevice newCommandQueue];

    // create global rotated uniform buffers
    for (int i = 0; i < GfxConfig::MaxInflightFrames; i++) {
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
    for (int i = 0; i < GfxConfig::MaxInflightFrames; i++) {
        dispatch_semaphore_wait(mtlInflightSemaphore, DISPATCH_TIME_FOREVER);
    }
    for (int i = 0; i < GfxConfig::MaxInflightFrames; i++) {
        this->uniformBuffers[i] = nil;
    }
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
        case GfxFeature::MSAARenderTargets:
        case GfxFeature::PackedVertexFormat_10_2:
        case GfxFeature::MultipleRenderTarget:
        case GfxFeature::Texture3D:
        case GfxFeature::TextureArray:
            return true;
        default:
            return false;
    }
}

//------------------------------------------------------------------------------
void
mtlRenderer::commitFrame() {
    o_assert_dbg(this->valid);
    o_assert_dbg(nil == this->curRenderCmdEncoder);
    o_assert_dbg(nil != this->curCommandBuffer);

    this->rpValid = false;

    // commit the global uniform buffer updates
    #if ORYOL_MACOS
    [this->uniformBuffers[this->curFrameRotateIndex] didModifyRange:NSMakeRange(0, this->curUniformBufferOffset)];
    #endif
    [this->curCommandBuffer presentDrawable:[osBridge::ptr()->mtkView currentDrawable]];
    __block dispatch_semaphore_t blockSema = mtlInflightSemaphore;
    [this->curCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
        dispatch_semaphore_signal(blockSema);
    }];
    [this->curCommandBuffer commit];

    // rotate to next uniform buffer
    if (++this->curFrameRotateIndex >= GfxConfig::MaxInflightFrames) {
        this->curFrameRotateIndex = 0;
    }

    this->frameIndex++;
    this->curUniformBufferOffset = 0;
    this->curRenderCmdEncoder = nil;
    this->curCommandBuffer = nil;
    this->curUniformBufferPtr = nullptr;
    this->curPipeline = nullptr;
    this->curPrimaryMesh = nullptr;
}

//------------------------------------------------------------------------------
const DisplayAttrs&
mtlRenderer::renderPassAttrs() const {
    o_assert_dbg(this->valid);
    return this->rpAttrs;
}

//------------------------------------------------------------------------------
void
mtlRenderer::applyViewPort(int x, int y, int width, int height, bool originTopLeft) {
    o_assert_dbg(this->valid);
    if (nil == this->curRenderCmdEncoder) {
        return;
    }

    MTLViewport vp;
    vp.originX = (double) x;
    vp.originY = (double) (originTopLeft ? y : (this->rpAttrs.FramebufferHeight - (y + height)));
    vp.width   = (double) width;
    vp.height  = (double) height;
    vp.znear   = 0.0;
    vp.zfar    = 1.0;
    [this->curRenderCmdEncoder setViewport:vp];
}

//------------------------------------------------------------------------------
void
mtlRenderer::applyScissorRect(int x, int y, int width, int height, bool originTopLeft) {
    o_assert_dbg(this->valid);
    o_assert_dbg(width >= 0);
    o_assert_dbg(height >= 0);

    if (nil == this->curRenderCmdEncoder) {
        return;
    }

    // clip against frame buffer size
    x = std::min(std::max(0, x), this->rpAttrs.FramebufferWidth - 1);
    y = std::min(std::max(0, y), this->rpAttrs.FramebufferHeight - 1);
    if ((x + width) > this->rpAttrs.FramebufferWidth) {
        width = this->rpAttrs.FramebufferWidth - x;
    }
    if ((y + height) > this->rpAttrs.FramebufferHeight) {
        height = this->rpAttrs.FramebufferHeight - y;
    }
    if (width <= 0) {
        width = 1;
    }
    if (height <= 0) {
        height = 1;
    }

    MTLScissorRect rect;
    rect.x = x;
    rect.y = originTopLeft ? y : this->rpAttrs.FramebufferHeight - (y + height);
    rect.width = width;
    rect.height = height;

    // need to clip against render target
    [this->curRenderCmdEncoder setScissorRect:rect];
}

//------------------------------------------------------------------------------
void
mtlRenderer::checkCreateCommandBuffer() {
    o_assert_dbg(this->valid);
    if (nil == this->curCommandBuffer) {
        // block until the oldest frame in flight has finished
        dispatch_semaphore_wait(mtlInflightSemaphore, DISPATCH_TIME_FOREVER);
        // get a new command buffer
        this->curCommandBuffer = [this->commandQueue commandBufferWithUnretainedReferences];
    }
}

//------------------------------------------------------------------------------
void
mtlRenderer::beginPass(renderPass* pass, const PassAction* action) {
    o_assert_dbg(this->valid);
    o_assert_dbg(nil == this->curRenderCmdEncoder);
    o_assert_dbg(action);

    // create command buffer if this is the first call in the current frame
    this->checkCreateCommandBuffer();

    // get the base pointer for the uniform buffer, this only happens once per frame
    if (nullptr == this->curUniformBufferPtr) {
        this->curUniformBufferPtr = (uint8*)[this->uniformBuffers[this->curFrameRotateIndex] contents];
    }

    // default, or offscreen render target?
    MTLRenderPassDescriptor* passDesc = nil;
    if (nullptr == pass) {
        // default render target
        passDesc = [osBridge::ptr()->mtkView currentRenderPassDescriptor];
        this->rpAttrs = this->pointers.displayMgr->GetDisplayAttrs();
    }
    else {
        passDesc = [MTLRenderPassDescriptor renderPassDescriptor];
        o_assert_dbg(pass->colorTextures[0]);
        this->rpAttrs = DisplayAttrs::FromTextureAttrs(pass->colorTextures[0]->textureAttrs);
    }
    if (passDesc) {
        this->rpValid = true;
    }
    else {
        // pass descriptor will not be valid if window is minimized
        return;
    }

    // initialize renderpass descriptor
    if (pass) {
        // offscreen, might be multiple-rendertarget
        for (int i = 0; i < GfxConfig::MaxNumColorAttachments; i++) {
            if (pass->colorTextures[i]) {
                const auto& colorAtt = pass->Setup.ColorAttachments[i];
                bool isMSAA = pass->colorTextures[i]->textureAttrs.SampleCount > 1;
                passDesc.colorAttachments[i].loadAction = mtlTypes::asLoadAction(action, i, false);
                passDesc.colorAttachments[i].storeAction = isMSAA ? MTLStoreActionMultisampleResolve:MTLStoreActionStore;
                const glm::vec4& c = action->Color[i];
                passDesc.colorAttachments[i].clearColor = MTLClearColorMake(c.x, c.y, c.z, c.w);
                if (isMSAA) {
                    // render to MSAA render target...
                    o_assert_dbg(pass->colorTextures[i]->mtlMSAATex);
                    passDesc.colorAttachments[i].texture = pass->colorTextures[i]->mtlMSAATex;
                    passDesc.colorAttachments[i].resolveTexture = pass->colorTextures[i]->mtlTextures[0];
                    passDesc.colorAttachments[i].resolveLevel = colorAtt.MipLevel;
                }
                else {
                    // render to non-MSAA render target...
                    passDesc.colorAttachments[i].texture = pass->colorTextures[i]->mtlTextures[0];
                    passDesc.colorAttachments[i].level = colorAtt.MipLevel;
                }
                switch (pass->colorTextures[i]->textureAttrs.Type) {
                    case TextureType::TextureCube:
                    case TextureType::TextureArray:
                        passDesc.colorAttachments[i].slice = pass->Setup.ColorAttachments[i].Slice;
                        break;
                    case TextureType::Texture3D:
                        passDesc.colorAttachments[i].depthPlane = pass->Setup.ColorAttachments[i].Slice;
                        break;
                    default:
                        break;
                }
            }
        }
        if (PixelFormat::IsDepthFormat(this->rpAttrs.DepthPixelFormat)) {
            passDesc.depthAttachment.texture = pass->depthStencilTexture->mtlDepthTex;
            passDesc.depthAttachment.loadAction = mtlTypes::asLoadAction(action, 0, true);
            passDesc.depthAttachment.clearDepth = action->Depth;
        }
        else if (PixelFormat::IsDepthStencilFormat(this->rpAttrs.DepthPixelFormat)) {
            passDesc.depthAttachment.texture = pass->depthStencilTexture->mtlDepthTex;
            passDesc.depthAttachment.loadAction = mtlTypes::asLoadAction(action, 0, true);
            passDesc.depthAttachment.clearDepth = action->Depth; 
            passDesc.stencilAttachment.texture = pass->depthStencilTexture->mtlDepthTex;
            passDesc.stencilAttachment.loadAction = mtlTypes::asLoadAction(action, 0, true);
            passDesc.stencilAttachment.clearStencil = action->Stencil;
        }
    }
    else {
        // default framebuffer
        passDesc.colorAttachments[0].loadAction = mtlTypes::asLoadAction(action, 0, false);
        const glm::vec4& c = action->Color[0];
        passDesc.colorAttachments[0].clearColor = MTLClearColorMake(c.x, c.y, c.z, c.w);
        if (PixelFormat::IsDepthFormat(this->gfxSetup.DepthFormat)) {
            passDesc.depthAttachment.loadAction = mtlTypes::asLoadAction(action, 0, true);
            passDesc.depthAttachment.clearDepth = action->Depth;
        }
        else if (PixelFormat::IsDepthStencilFormat(this->gfxSetup.DepthFormat)) {
            passDesc.depthAttachment.loadAction = mtlTypes::asLoadAction(action, 0, true);
            passDesc.depthAttachment.clearDepth = action->Depth;
            passDesc.stencilAttachment.loadAction = mtlTypes::asLoadAction(action, 0, true);
            passDesc.stencilAttachment.clearStencil = action->Stencil;
        }
    }

    // create command encoder for this render pass
    // this might return nil if the window is minimized
    this->curRenderCmdEncoder = [this->curCommandBuffer renderCommandEncoderWithDescriptor:passDesc];
    if (nil != this->curRenderCmdEncoder) {
        for (int bindSlot = 0; bindSlot < GfxConfig::MaxNumUniformBlocksPerStage; bindSlot++) {
            [this->curRenderCmdEncoder
                setVertexBuffer:this->uniformBuffers[this->curFrameRotateIndex]
                offset:0
                atIndex:bindSlot];
            [this->curRenderCmdEncoder
                setFragmentBuffer:this->uniformBuffers[this->curFrameRotateIndex]
                offset:0
                atIndex:bindSlot];
        }
    }
}

//------------------------------------------------------------------------------
void
mtlRenderer::endPass() {
    o_assert_dbg(this->valid);
    if (nil != this->curRenderCmdEncoder) {
        [this->curRenderCmdEncoder endEncoding];
        this->curRenderCmdEncoder = nil;
    }
}

//------------------------------------------------------------------------------
void
mtlRenderer::applyDrawState(pipeline* pip, mesh** meshes, int numMeshes) {
    o_assert_dbg(this->valid);
    o_assert_dbg(pip);
    o_assert_dbg(meshes && (numMeshes > 0));

    if (nil == this->curRenderCmdEncoder) {
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
    o_assert2_dbg(pip->Setup.BlendState.ColorFormat == this->rpAttrs.ColorPixelFormat, "ColorFormat in BlendState must match current render target!\n");
    o_assert2_dbg(pip->Setup.BlendState.DepthFormat == this->rpAttrs.DepthPixelFormat, "DepthFormat in BlendSTate must match current render target!\n");
    o_assert2_dbg(pip->Setup.RasterizerState.SampleCount == this->rpAttrs.SampleCount, "SampleCount in RasterizerState must match current render target!\n");

    // need to store draw state and first mesh for later draw call
    this->curPipeline = pip;
    this->curPrimaryMesh = meshes[0];
    o_assert_dbg(this->curPrimaryMesh);

    // apply general state
    const glm::vec4& bc = pip->Setup.BlendColor;
    const RasterizerState& rs = pip->Setup.RasterizerState;
    const DepthStencilState& dss = pip->Setup.DepthStencilState;
    [this->curRenderCmdEncoder setBlendColorRed:bc.x green:bc.y blue:bc.z alpha:bc.w];
    [this->curRenderCmdEncoder setCullMode:mtlTypes::asCullMode(rs.CullFaceEnabled, rs.CullFace)];
    [this->curRenderCmdEncoder setStencilReferenceValue:dss.StencilRef];

    // apply state objects
    [this->curRenderCmdEncoder setRenderPipelineState:pip->mtlRenderPipelineState];
    [this->curRenderCmdEncoder setDepthStencilState:pip->mtlDepthStencilState];

    // apply vertex buffers
    for (int meshIndex = 0; meshIndex < GfxConfig::MaxNumInputMeshes; meshIndex++) {
        const mesh* msh = meshIndex < numMeshes ? meshes[meshIndex] : nullptr;
        // NOTE: vertex buffers are located after constant buffers
        const int vbSlotIndex = meshIndex + GfxConfig::MaxNumUniformBlocksPerStage;
        if (msh) {
            // note: vb.mtlBuffers[vb.activeSlot] can be nil!
            const auto& vb = msh->buffers[mesh::vb];
            [this->curRenderCmdEncoder setVertexBuffer:vb.mtlBuffers[vb.activeSlot] offset:0 atIndex:vbSlotIndex];
        }
        else {
            [this->curRenderCmdEncoder setVertexBuffer:nil offset:0 atIndex:vbSlotIndex];
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
mtlRenderer::applyUniformBlock(ShaderStage::Code bindStage, int bindSlot, uint32_t typeHash, const uint8_t* ptr, int byteSize) {
    o_assert_dbg(this->valid);
    if (nil == this->curRenderCmdEncoder) {
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
    o_assert(InvalidIndex != ubIndex);
    const uint32_t ubTypeHash = shd->Setup.UniformBlockTypeHash(ubIndex);
    const int ubByteSize = shd->Setup.UniformBlockByteSize(ubIndex);
    o_assert(ubTypeHash == typeHash);
    o_assert(ubByteSize >= byteSize);
     o_assert2_dbg((this->curUniformBufferOffset + byteSize) <= this->gfxSetup.GlobalUniformBufferSize, "Global uniform buffer exhausted!\n");
    o_assert_dbg((this->curUniformBufferOffset & (MtlUniformAlignment-1)) == 0);
    #endif

    // write uniforms into global uniform buffer, advance buffer offset
    // and set current uniform buffer location on command-encoder
    // NOTE: we'll call didModifyRange only ONCE inside commitFrame!
    uint8* dstPtr = this->curUniformBufferPtr + this->curUniformBufferOffset;
    std::memcpy(dstPtr, ptr, byteSize);
    if (ShaderStage::VS == bindStage) {
        [this->curRenderCmdEncoder setVertexBufferOffset:this->curUniformBufferOffset atIndex:bindSlot];
    }
    else {
        [this->curRenderCmdEncoder setFragmentBufferOffset:this->curUniformBufferOffset atIndex:bindSlot];
    }
    this->curUniformBufferOffset = Memory::RoundUp(this->curUniformBufferOffset + byteSize, MtlUniformAlignment);
}

//------------------------------------------------------------------------------
void
mtlRenderer::applyTextures(ShaderStage::Code bindStage, texture** textures, int numTextures) {
    o_assert_dbg(this->valid);
    o_assert_dbg(((ShaderStage::VS == bindStage) && (numTextures <= GfxConfig::MaxNumVertexTextures)) ||
                 ((ShaderStage::FS == bindStage) && (numTextures <= GfxConfig::MaxNumFragmentTextures)));
    if (nil == this->curRenderCmdEncoder) {
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
            texture* tex = textures[i];
            [this->curRenderCmdEncoder setVertexTexture:tex->mtlTextures[tex->activeSlot] atIndex:i];
            [this->curRenderCmdEncoder setVertexSamplerState:tex->mtlSamplerState atIndex:i];
        }
    }
    else {
        for (int i = 0; i < numTextures; i++) {
            texture* tex = textures[i];
            [this->curRenderCmdEncoder setFragmentTexture:tex->mtlTextures[tex->activeSlot] atIndex:i];
            [this->curRenderCmdEncoder setFragmentSamplerState:tex->mtlSamplerState atIndex:i];
        }
    }
}

//------------------------------------------------------------------------------
void
mtlRenderer::draw(int baseElementIndex, int numElements, int numInstances) {
    o_assert_dbg(this->valid);
    if (nil == this->curRenderCmdEncoder) {
        return;
    }
    if (nullptr == this->curPipeline) {
        return;
    }
    const mesh* msh = this->curPrimaryMesh;
    o_assert_dbg(msh);
    if (IndexType::None == msh->indexBufferAttrs.Type) {
        [this->curRenderCmdEncoder drawPrimitives:(MTLPrimitiveType)this->curMTLPrimitiveType
            vertexStart:baseElementIndex
            vertexCount:numElements
            instanceCount:numInstances];
    }
    else {
        const auto& ib = msh->buffers[mesh::ib];
        o_assert_dbg(nil != ib.mtlBuffers[ib.activeSlot]);
        NSUInteger indexBufferOffset = baseElementIndex * IndexType::ByteSize(msh->indexBufferAttrs.Type);
        [this->curRenderCmdEncoder drawIndexedPrimitives:(MTLPrimitiveType)this->curMTLPrimitiveType
            indexCount:numElements
            indexType:(MTLIndexType)this->curMTLIndexType
            indexBuffer:ib.mtlBuffers[ib.activeSlot]
            indexBufferOffset:indexBufferOffset
            instanceCount:numInstances ];
    }
}

//------------------------------------------------------------------------------
void
mtlRenderer::draw(int primGroupIndex, int numInstances) {
    if (nil == this->curRenderCmdEncoder) {
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
    this->draw(primGroup.BaseElement, primGroup.NumElements, numInstances);
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
    o_assert_dbg(offsetsAndSizes.NumMipMaps <= attrs.NumMipMaps);
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

} // namespace _priv
} // namespace Oryol
