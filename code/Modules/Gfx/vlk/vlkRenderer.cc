//------------------------------------------------------------------------------
//  vlkRenderer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "vlkRenderer.h"
#include "Core/Assertion.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
vlkRenderer::vlkRenderer() :
valid(false) {
    // empty
}

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
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
vlkRenderer::discard() {
    o_assert_dbg(this->valid);
    this->pointers = gfxPointers();
    this->valid = false;
    o_error("FIXME!\n");
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
    o_error("FIXME!\n");
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
    o_error("FIXME!\n");
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
