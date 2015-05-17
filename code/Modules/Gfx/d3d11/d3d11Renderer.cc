//------------------------------------------------------------------------------
//  d3d11Renderer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11Renderer.h"
#include <glm/mat4x4.hpp>

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d11Renderer::d3d11Renderer() {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
d3d11Renderer::~d3d11Renderer() {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
d3d11Renderer::setup(meshPool* mshPool) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
d3d11Renderer::discard() {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
bool
d3d11Renderer::isValid() const {
    o_error("FIXME!\n");
    return false;
}

//------------------------------------------------------------------------------
void
d3d11Renderer::resetStateCache() {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
bool
d3d11Renderer::supports(GfxFeature::Code feat) const {
    o_error("FIXME!\n");
    return false;
}

//------------------------------------------------------------------------------
void
d3d11Renderer::commitFrame() {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
const DisplayAttrs&
d3d11Renderer::renderTargetAttrs() const {
    o_error("FIXME!\n");
    static const DisplayAttrs attrs;
    return attrs;
}

//------------------------------------------------------------------------------
void
d3d11Renderer::applyRenderTarget(displayMgr* displayManager, texture* rt) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::applyViewPort(int32 x, int32 y, int32 width, int32 height) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::applyScissorRect(int32 x, int32 y, int32 width, int32 height) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
d3d11Renderer::applyBlendColor(const glm::vec4& color) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
d3d11Renderer::applyDrawState(drawState* ds) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void
d3d11Renderer::applyTexture(int32 index, const texture* tex) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
template<> void
d3d11Renderer::applyVariable(int32 index, const float32& val) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
template<> void
d3d11Renderer::applyVariable(int32 index, const glm::vec2& val) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
template<> void
d3d11Renderer::applyVariable(int32 index, const glm::vec3& val) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
template<> void
d3d11Renderer::applyVariable(int32 index, const glm::vec4& val) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
template<> void
d3d11Renderer::applyVariable(int32 index, const int32& val) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
template<> void
d3d11Renderer::applyVariable(int32 index, const glm::ivec2& val) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
template<> void
d3d11Renderer::applyVariable(int32 index, const glm::ivec3& val) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
template<> void
d3d11Renderer::applyVariable(int32 index, const glm::ivec4& val) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
template<> void
d3d11Renderer::applyVariable(int32 index, const glm::mat4& val) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
template<> void
d3d11Renderer::applyVariable(int32 index, const glm::mat3& val) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
template<> void
d3d11Renderer::applyVariable(int32 index, const glm::mat2& val) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
template<> void
d3d11Renderer::applyVariableArray(int32 index, const float32* values, int32 numValues) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
template<> void
d3d11Renderer::applyVariableArray(int32 index, const glm::vec2* values, int32 numValues) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
template<> void
d3d11Renderer::applyVariableArray(int32 index, const glm::vec3* values, int32 numValues) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
template<> void
d3d11Renderer::applyVariableArray(int32 index, const glm::vec4* values, int32 numValues) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
template<> void
d3d11Renderer::applyVariableArray(int32 index, const int32* values, int32 numValues) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
template<> void
d3d11Renderer::applyVariableArray(int32 index, const glm::ivec2* values, int32 numValues) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
template<> void
d3d11Renderer::applyVariableArray(int32 index, const glm::ivec3* values, int32 numValues) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
template<> void
d3d11Renderer::applyVariableArray(int32 index, const glm::ivec4* values, int32 numValues) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
template<> void
d3d11Renderer::applyVariableArray(int32 index, const glm::mat4* values, int32 numValues) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
template<> void
d3d11Renderer::applyVariableArray(int32 index, const glm::mat3* values, int32 numValues) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
template<> void
d3d11Renderer::applyVariableArray(int32 index, const glm::mat2* values, int32 numValues) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::clear(PixelChannel::Mask channels, const glm::vec4& color, float32 depth, uint8 stencil) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::draw(int32 primGroupIndex) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::draw(const PrimitiveGroup& primGroup) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::drawInstanced(int32 primGroupIndex, int32 numInstances) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::drawInstanced(const PrimitiveGroup& primGroup, int32 numInstances) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::updateVertices(mesh* msh, const void* data, int32 numBytes) {
    o_error("FIXME!\n");
}

//------------------------------------------------------------------------------
void 
d3d11Renderer::readPixels(displayMgr* displayManager, void* buf, int32 bufNumBytes) {
    o_error("FIXME!\n");
}

} // namespace _priv
} // namespace Oryol