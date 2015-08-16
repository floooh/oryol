//------------------------------------------------------------------------------
//  d3d12Renderer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12Renderer.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d12Renderer::d3d12Renderer() :
valid(false) {
    o_warn("d3d12Renderer::d3d12Renderer()\n");
}

//------------------------------------------------------------------------------
d3d12Renderer::~d3d12Renderer() {
    o_assert_dbg(!this->valid);
}

//------------------------------------------------------------------------------
void
d3d12Renderer::setup(const GfxSetup& setup, const gfxPointers& ptrs) {
    o_assert_dbg(!this->valid);

    this->valid = true;
    this->pointers = ptrs;

    o_warn("d3d12Renderer::setup()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::discard() {
    o_assert_dbg(this->valid);
    
    o_warn("d3d12Renderer::discard()\n");

    this->pointers = gfxPointers();
    this->valid = false;
}

//------------------------------------------------------------------------------
bool
d3d12Renderer::isValid() const {
    return this->valid;
}

//------------------------------------------------------------------------------
void
d3d12Renderer::resetStateCache() {
    o_warn("d3d12Renderer::resetStateCache()\n");
}

//------------------------------------------------------------------------------
bool
d3d12Renderer::queryFeature(GfxFeature::Code feat) const {
    switch (feat) {
        case GfxFeature::TextureCompressionDXT:
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
d3d12Renderer::commitFrame() {
    o_assert_dbg(this->valid);
    o_warn("d3d12Renderer::commitFrame()\n");
}

//------------------------------------------------------------------------------
const DisplayAttrs&
d3d12Renderer::renderTargetAttrs() const {
    o_warn("d3d12Renderer::renderTargetAttrs()\n");
    static DisplayAttrs dummyAttrs;
    return dummyAttrs;
}

//------------------------------------------------------------------------------
void
d3d12Renderer::applyRenderTarget(texture* rt, const ClearState& clearState) {
    o_warn("d3d12Renderer::applyRenderTarget()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::applyViewPort(int32 x, int32 y, int32 width, int32 height, bool originTopLeft) {
    o_warn("d3d12Renderer::applyViewPort()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::applyScissorRect(int32 x, int32 y, int32 width, int32 height, bool originTopLeft) {
    o_warn("d3d12Renderer::applyScissorRect()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::applyDrawState(drawState* ds) {
    o_warn("d3d12Renderer::applyDrawState()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::applyUniformBlock(int32 blockIndex, int64 layoutHash, const uint8* ptr, int32 byteSize) {
    o_warn("d3d12Renderer::applyUniformBlock()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::draw(const PrimitiveGroup& primGroup) {
    o_warn("d3d12Renderer::draw()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::draw(int32 primGroupIndex) {
    o_warn("d3d12Renderer::draw()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::drawInstanced(const PrimitiveGroup& primGroup, int32 numInstances) {
    o_warn("d3d12Renderer::drawInstanced()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::drawInstanced(int32 primGroupIndex, int32 numInstances) {
    o_warn("d3d12Renderer::drawInstanced()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::updateVertices(mesh* msh, const void* data, int32 numBytes) {
    o_warn("d3d12Renderer::updateVertices()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::updateIndices(mesh* msh, const void* data, int32 numBytes) {
    o_warn("d3d12Renderer::updateIndices()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::readPixels(void* buf, int32 bufNumBytes) {
    o_warn("d3d12Renderer::readPixels()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::invalidateMeshState() {
    o_warn("d3d12Renderer::invalidateMeshState()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::invalidateShaderState() {
    o_warn("d3d12Renderer::invalidateShaderState()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::invalidateDrawState() {
    o_warn("d3d12Renderer::invalidateDrawState()\n");
}

//------------------------------------------------------------------------------
void
d3d12Renderer::invalidateTextureState() {
    o_warn("d3d12Renderer::invalidateTextureState()\n");
}

} // namespace _priv
} // namespace Oryol

