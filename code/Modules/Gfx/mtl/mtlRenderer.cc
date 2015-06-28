//------------------------------------------------------------------------------
//  mtlRenderer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlRenderer.h"

namespace Oryol {
namespace _priv {

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

    // FIXME
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

    o_error("mtlRenderer::commitFrame()\n");
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

    o_error("mtlRenderer::applyRenderTarget()\n");
}

//------------------------------------------------------------------------------
void
mtlRenderer::applyDrawState(drawState* ds) {
    o_assert_dbg(this->valid);

    o_error("mtlRenderer::applyDrawState()\n");
}

//------------------------------------------------------------------------------
void
mtlRenderer::clear(ClearTarget::Mask clearMask, const glm::vec4& color, float32 depth, uint8 stencil) {
    o_assert_dbg(this->valid);

    o_error("mtlRenderer::clear()\n");
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
