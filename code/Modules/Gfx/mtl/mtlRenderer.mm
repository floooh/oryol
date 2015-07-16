//------------------------------------------------------------------------------
//  mtlRenderer.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlRenderer.h"
#include "mtlTypes.h"
#include "Gfx/Core/displayMgr.h"
#include "Gfx/Resource/resourcePools.h"
#include "Gfx/Core/UniformLayout.h"
#include "Gfx/Resource/drawState.h"
#include "Gfx/Resource/mesh.h"
#include "Gfx/Resource/programBundle.h"
#include "Gfx/Resource/texture.h"

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
curCommandEncoder(nil),
curUniformBufferIndex(0),
curUniformBufferOffset(0) {
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
    dispatch_semaphore_signal(mtlInflightSemaphore);

    // setup central metal objects
    this->mtlDevice = this->pointers.displayMgr->cocoa.mtlDevice;
    this->commandQueue = [this->mtlDevice newCommandQueue];
    this->commandQueue.label = @"OryolCommandQueue";

    // create global rotated uniform buffer
    for (int i = 0; i < MaxNumUniformBuffers; i++) {
        // FIXME: is options:0 right? this is used by the Xcode game sample
        this->uniformBuffers[i] = [this->mtlDevice newBufferWithLength:setup.GlobalUniformBufferSize options:0];
    }
    this->curUniformBufferIndex = 0;
    this->curUniformBufferOffset = 0;
}

//------------------------------------------------------------------------------
void
mtlRenderer::discard() {
    o_assert_dbg(this->valid);

    for (int i = 0; i < MaxNumUniformBuffers; i++) {
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

    // commit the global uniform buffer updates
    [this->uniformBuffers[this->curUniformBufferIndex] didModifyRange:NSMakeRange(0, this->curUniformBufferOffset)];

    // wait for previous frame to finish
    dispatch_semaphore_wait(mtlInflightSemaphore, DISPATCH_TIME_FOREVER);

    __block dispatch_semaphore_t blockSema = mtlInflightSemaphore;
    [this->curCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
        dispatch_semaphore_signal(blockSema);
    }];

    [this->curCommandEncoder endEncoding];
    [this->curCommandBuffer presentDrawable:curDrawable];
    [this->curCommandBuffer commit];

    // rotate to next uniform buffer
    if (++this->curUniformBufferIndex >= MaxNumUniformBuffers) {
        this->curUniformBufferIndex = 0;
    }
    this->curUniformBufferOffset = 0;

    // FIXME: probably not a good idea to wait here right after the commit!
    this->curCommandEncoder = nil;
    this->curCommandBuffer = nil;
    this->curDrawable = nil;
}

//------------------------------------------------------------------------------
const DisplayAttrs&
mtlRenderer::renderTargetAttrs() const {
    o_assert_dbg(this->valid);
    return this->rtAttrs;
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
        return;
    }

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
    const int numMeshSlots = ds->meshes.Size();
    for (int meshIndex = 0; meshIndex < numMeshSlots; meshIndex++) {
        const mesh* msh = ds->meshes[meshIndex];
        // NOTE: vertex buffers are located after constant buffers
        const int vbSlotIndex = meshIndex + GfxConfig::MaxNumUniformBlocks;
        if (msh) {
            o_assert_dbg(msh->mtlVertexBuffers[msh->activeVertexBufferSlot]);
            [this->curCommandEncoder setVertexBuffer:msh->mtlVertexBuffers[msh->activeVertexBufferSlot] offset:0 atIndex:vbSlotIndex];
        }
        else {
            [this->curCommandEncoder setVertexBuffer:nil offset:0 atIndex:vbSlotIndex];
        }
    }
}

//------------------------------------------------------------------------------
void
mtlRenderer::applyUniformBlock(int32 blockIndex, int64 layoutHash, const uint8* ptr, int32 byteSize) {
    o_assert_dbg(this->valid);
    o_assert_dbg(this->curCommandEncoder);
    if (nullptr == this->curDrawState) {
        return;
    }

    // get the uniform layout object for this uniform block
    const programBundle* prog = this->curDrawState->prog;
    o_assert_dbg(prog);
    const UniformLayout& layout = prog->Setup.UniformBlockLayout(blockIndex);

    // check whether the provided struct is type-compatible with the uniform layout
    o_assert2(layout.TypeHash == layoutHash, "incompatible uniform block!\n");

    // set textures and samplers, and find start of uniform buffer data
    // FIXME: textures should be separated from uniforms, probably even go
    // into the drawState (although this would not allow to change textures
    // between instances)
    const ShaderType::Code bindShaderStage = prog->getUniformBlockShaderStage(blockIndex);
    const int32 bindSlotIndex = prog->getUniformBlockBindSlotIndex(blockIndex);
    const uint8* uBufferPtr = nullptr;
    const int numComps = layout.NumComponents();
    for (int compIndex = 0; compIndex < numComps; compIndex++) {
        const auto& comp = layout.ComponentAt(compIndex);
        if (comp.Type == UniformType::Texture) {
            const int32 texBindSlotIndex = comp.BindSlotIndex;
            o_assert_dbg(texBindSlotIndex != InvalidIndex);
            const uint8* valuePtr = ptr + layout.ComponentByteOffset(compIndex);
            const Id& resId = *(const Id*)valuePtr;
            const texture* tex = this->pointers.texturePool->Lookup(resId);
            o_assert_dbg(tex);
            o_assert_dbg(tex->mtlTex);
            o_assert_dbg(tex->mtlSamplerState);
            if (ShaderType::VertexShader == bindShaderStage) {
                [this->curCommandEncoder setVertexTexture:tex->mtlTex atIndex:texBindSlotIndex];
                [this->curCommandEncoder setVertexSamplerState:tex->mtlSamplerState atIndex:texBindSlotIndex];
            }
            else {
                [this->curCommandEncoder setFragmentTexture:tex->mtlTex atIndex:texBindSlotIndex];
                [this->curCommandEncoder setFragmentSamplerState:tex->mtlSamplerState atIndex:texBindSlotIndex];
            }
        }
        else {
            // found the start of the cbuffer struct
            uBufferPtr = ptr + layout.ComponentByteOffset(compIndex);
            break;
        }
    }

    // write uniforms into global uniform buffer, advance buffer offset
    // and set current uniform buffer location on command-encoder
    // NOTE: we'll call didModifyRange only ONCE inside CommitFrame!
    if (nullptr != uBufferPtr) {
        id<MTLBuffer> mtlBuffer = this->uniformBuffers[this->curUniformBufferIndex];
        const int32 uniformSize = layout.ByteSizeWithoutTextures();
        o_assert2((this->curUniformBufferOffset + uniformSize) <= this->gfxSetup.GlobalUniformBufferSize, "Global uniform buffer exhausted!\n");
        uint8* dstPtr = ((uint8*)[mtlBuffer contents]) + this->curUniformBufferOffset;
        Memory::Copy(ptr, dstPtr, uniformSize);

        // set constant buffer location for next draw call
        if (ShaderType::VertexShader == bindShaderStage) {
            [this->curCommandEncoder setVertexBuffer:mtlBuffer offset:this->curUniformBufferOffset atIndex:bindSlotIndex];
        }
        else {
            [this->curCommandEncoder setFragmentBuffer:mtlBuffer offset:this->curUniformBufferOffset atIndex:bindSlotIndex];
        }

        // advance uniform buffer offset
        this->curUniformBufferOffset += uniformSize;
    }
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
    o_assert_dbg(nullptr != msh);
    o_assert_dbg(nullptr != data);
    o_assert_dbg(numBytes > 0);

    const VertexBufferAttrs& attrs = msh->vertexBufferAttrs;
    const Usage::Code vbUsage = attrs.BufferUsage;
    o_assert_dbg((numBytes > 0) && (numBytes <= attrs.ByteSize()));
    o_assert_dbg(vbUsage == Usage::Stream);
    
    uint8 slotIndex = msh->activeVertexBufferSlot;
    if (Usage::Stream == vbUsage) {
        // if usage is streaming, rotate slot index to next dynamic vertex buffer
        // to implement double/multi-buffering
        slotIndex++;
        if (slotIndex >= mesh::NumSlots) {
            slotIndex = 0;
        }
        msh->activeVertexBufferSlot = slotIndex;
    }

    id<MTLBuffer> mtlBuffer = msh->mtlVertexBuffers[slotIndex];
    o_assert_dbg(numBytes <= int([mtlBuffer length]));
    void* dstPtr = [mtlBuffer contents];
    Memory::Copy(data, dstPtr, numBytes);
    [mtlBuffer didModifyRange:NSMakeRange(0, numBytes)];
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

//------------------------------------------------------------------------------
void
mtlRenderer::invalidateTextureState() {
    // FIXME!
    Log::Info("mtlRenderer::invalidateTextureState()\n");
}

} // namespace _priv
} // namespace Oryol
