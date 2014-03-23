#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Renderer::glRenderMgr
    @brief GL implementation of renderer class
*/
#include "Render/base/renderMgrBase.h"

namespace Oryol {
namespace Render {
    
class glRenderMgr : public renderMgrBase {
public:
    /// apply the current mesh object
    bool ApplyMesh(mesh* mesh);
    /// apply the current program object
    bool ApplyProgram(programBundle* progBundle, uint32 selectionMask);    
    /// apply a shader variable
    template<class T> void ApplyVariable(int32 index, const T& value);
    /// apply a shader variable array
    template<class T> void ApplyVariableArray(int32 index, const T* values, int32 numValues);
    /// clear the currently assigned render target
    void Clear(bool color, bool depth, bool stencil);
    /// submit a draw call with primitive group index in current mesh
    void Draw(int32 primGroupIndex);
    /// submit a draw call with overridden primitive group
    void Draw(const PrimitiveGroup& primGroup);
};

} // namespace Render
} // namespace Oryol