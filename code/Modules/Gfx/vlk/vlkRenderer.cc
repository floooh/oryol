//------------------------------------------------------------------------------
//  vlkRenderer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "vlkRenderer.h"
#include "Core/Assertion.h"
#include "Gfx/Core/displayMgr.h"
#include "Gfx/Core/gfxPointers.h"
#include "vlk_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
vlkRenderer::~vlkRenderer() {
    o_assert_dbg(!this->valid);
}

//------------------------------------------------------------------------------
void
vlkRenderer::setup(const GfxSetup& setup, const gfxPointers& ptrs) {
    o_assert_dbg(!this->valid);

    this->valid = true;
    this->pointers = ptrs;
    this->context = &ptrs.displayMgr->vlkContext;    
}

//------------------------------------------------------------------------------
void
vlkRenderer::discard() {
    o_assert_dbg(this->valid);
    o_assert_dbg(this->context);
    o_assert_dbg(this->context->Device);

    vkDeviceWaitIdle(this->context->Device);
    this->pointers = gfxPointers();
    this->valid = false;
}

//------------------------------------------------------------------------------
bool
vlkRenderer::isValid() const {
    return this->valid;
}

//------------------------------------------------------------------------------
void
vlkRenderer::resetStateCache() {
    o_assert_dbg(this->valid);
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
bool
vlkRenderer::queryFeature(GfxFeature::Code feat) const {
    o_error("FIXME!\n");
    return false;
}

//------------------------------------------------------------------------------
void
vlkRenderer::commitFrame() {
    o_assert_dbg(this->valid);
    o_assert_dbg(this->cmdBuf);
    o_assert_dbg(this->context);
    o_assert_dbg(this->context->Queue);

    vkCmdEndRenderPass(this->cmdBuf);

    // transition swapchain image back from attachment to present state
    VkImageMemoryBarrier prePresentBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
    prePresentBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    prePresentBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    prePresentBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    prePresentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    prePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    prePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    prePresentBarrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    prePresentBarrier.image = this->context->curSwapChainImage();
    VkImageMemoryBarrier* imageBarriers = &prePresentBarrier;
    vkCmdPipelineBarrier(this->cmdBuf,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,         // srcStateMask
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,       // dstStageMask
        0,                                          // dependencyFlags
        0, nullptr,                                 // memoryBarrierCount, pMemoryBarriers
        0, nullptr,                                 // bufferMemoryBarrierCount, pBufferMemoryBarriers
        1, imageBarriers);                          // imageMemoryBarrierCount, pImageMemoryBarriers

    // submit command buffer and present the frame
    this->context->present();
    this->cmdBuf = nullptr;
}

//------------------------------------------------------------------------------
const DisplayAttrs&
vlkRenderer::renderTargetAttrs() const {
    return this->rtAttrs;
}

//------------------------------------------------------------------------------
void
vlkRenderer::applyRenderTarget(texture* rt, const ClearState& clearState) {
    o_assert_dbg(this->valid);
    o_assert_dbg(this->context);
    o_assert_dbg(this->context->RenderPass);

    // first call in the frame?
    if (!this->cmdBuf) {
        this->cmdBuf = this->context->beginFrame();
    }

    VkRenderPass renderPass = nullptr;
    VkFramebuffer frameBuffer = nullptr;
    if (nullptr == rt) {
        this->rtAttrs = this->pointers.displayMgr->GetDisplayAttrs();
        renderPass = this->context->RenderPass;
        frameBuffer = this->context->curFramebuffer();

        // transition swapchain image from present to attachment state
        VkImage swapChainImage = this->context->curSwapChainImage();
        vlkResAllocator::transitionImageLayout(
            this->cmdBuf,
            swapChainImage, 
            VK_IMAGE_ASPECT_COLOR_BIT, 
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    }
    else {
        o_error("FIXME!\n");
    }
    o_assert_dbg(this->rtAttrs.FramebufferWidth > 0);
    o_assert_dbg(this->rtAttrs.FramebufferHeight > 0);
    o_assert_dbg(renderPass);
    o_assert_dbg(frameBuffer);

    VkClearValue clearValues[2] = { };
    clearValues[0].color.float32[0] = clearState.Color.r;
    clearValues[0].color.float32[1] = clearState.Color.g;
    clearValues[0].color.float32[2] = clearState.Color.b;
    clearValues[0].color.float32[3] = clearState.Color.a;
    clearValues[1].depthStencil.depth = clearState.Depth;
    clearValues[1].depthStencil.stencil = clearState.Stencil;

    VkRenderPassBeginInfo rpBegin = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
    rpBegin.renderPass = renderPass;
    rpBegin.framebuffer = frameBuffer;
    rpBegin.renderArea.offset.x = 0;
    rpBegin.renderArea.offset.y = 0;
    rpBegin.renderArea.extent.width = this->rtAttrs.FramebufferWidth;
    rpBegin.renderArea.extent.height = this->rtAttrs.FramebufferHeight;
    rpBegin.clearValueCount = 2;
    rpBegin.pClearValues = clearValues;
    vkCmdBeginRenderPass(this->cmdBuf, &rpBegin, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport vp = { };
    vp.width = (float) this->rtAttrs.FramebufferWidth;
    vp.height = (float) this->rtAttrs.FramebufferHeight;
    vp.minDepth = 0.0f;
    vp.maxDepth = 1.0f;
    vkCmdSetViewport(this->cmdBuf, 0, 1, &vp);

    VkRect2D scissor = { };
    scissor.extent.width = this->rtAttrs.FramebufferWidth;
    scissor.extent.height = this->rtAttrs.FramebufferHeight;
    vkCmdSetScissor(this->cmdBuf, 0, 1, &scissor);
}

//------------------------------------------------------------------------------
void
vlkRenderer::applyViewPort(int32 x, int32 y, int32 width, int32 height, bool originTopLeft) {
    o_assert_dbg(this->valid);
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
vlkRenderer::applyScissorRect(int32 x, int32 y, int32 width, int32 height, bool originTopLeft) {
    o_assert_dbg(this->valid);
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
vlkRenderer::applyDrawState(pipeline* pip, mesh** meshes, int numMeshes) {
    o_assert_dbg(this->valid);
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
vlkRenderer::applyUniformBlock(ShaderStage::Code bindStage, int32 bindSlot, int64 hashLayout, const uint8* ptr, int32 byteSize) {
    o_assert_dbg(this->valid);
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
vlkRenderer::applyTextures(ShaderStage::Code bindStage, texture** textures, int32 numTextures) {
    o_assert_dbg(this->valid);
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
vlkRenderer::draw(const PrimitiveGroup& primGroup) {
    o_assert_dbg(this->valid);
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
vlkRenderer::draw(int32 primGroupIndex) {
    o_assert_dbg(this->valid);
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
vlkRenderer::drawInstanced(const PrimitiveGroup& primGroup, int32 numInstances) {
    o_assert_dbg(this->valid);
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
vlkRenderer::drawInstanced(int32 primGroupIndex, int32 numInstances) {
    o_assert_dbg(this->valid);
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
vlkRenderer::updateVertices(mesh* msh, const void* data, int32 numBytes) {
    o_assert_dbg(this->valid);
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
vlkRenderer::updateIndices(mesh* msh, const void* data, int32 numBytes) {
    o_assert_dbg(this->valid);
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
vlkRenderer::updateTexture(texture* tex, const void* data, const ImageDataAttrs& offsetsAndSizes) {
    o_assert_dbg(this->valid);
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
vlkRenderer::readPixels(void* buf, int32 bufNumBytes) {
    o_error("vlkRenderer::readPixels() NOT IMPLEMENTED!\n");
}

//------------------------------------------------------------------------------
void
vlkRenderer::invalidateMeshState() {
    o_assert_dbg(this->valid);
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
vlkRenderer::invalidateShaderState() {
    o_assert_dbg(this->valid);
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
vlkRenderer::invalidatePipeline() {
    o_assert_dbg(this->valid);
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
vlkRenderer::invalidateTextureState() {
    o_assert_dbg(this->valid);
    o_error("FIXME!\n");
}

} // namespace _priv
} // namespace Oryol
