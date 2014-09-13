#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glRenderMgr
    @ingroup _priv
    @brief GL implementation of renderer class
*/
#include "Render/base/renderMgrBase.h"
#include "glm/vec4.hpp"

namespace Oryol {
namespace _priv {
    
class glRenderMgr : public renderMgrBase {
public:
    /// test if an optional feature is supported
    bool Supports(RenderFeature::Code feat) const;
    /// apply the current render target
    void ApplyRenderTarget(texture* rt);
    /// apply draw state to use for rendering
    void ApplyDrawState(drawState* ds);
    /// apply a texture sampler variable (special case)
    void ApplyTexture(int32 index, const texture* tex);
    /// apply a shader variable
    template<class T> void ApplyVariable(int32 index, const T& value);
    /// apply a shader variable array
    template<class T> void ApplyVariableArray(int32 index, const T* values, int32 numValues);
    /// clear the currently assigned render target
    void Clear(PixelChannel::Mask channels, const glm::vec4& color, float32 depth, uint8 stencil);
    /// submit a draw call with primitive group index in current mesh
    void Draw(int32 primGroupIndex);
    /// submit a draw call with direct primitive group
    void Draw(const PrimitiveGroup& primGroup);
    /// submit a draw call for instanced rendering with primitive group index in current mesh
    void DrawInstanced(int32 primGroupIndex, int32 numInstances);
    /// submit a draw call for instanced rendering with direct primitive group
    void DrawInstanced(const PrimitiveGroup& primGroup, int32 numInstances);
    /// update vertex data
    void UpdateVertices(mesh* msh, int32 numBytes, const void* data);
    /// read pixels back from framebuffer, causes a PIPELINE STALL!!!
    void ReadPixels(void* buf, int32 bufNumBytes);
};

} // namespace _priv
} // namespace Oryol