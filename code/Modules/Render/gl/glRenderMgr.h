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
};
    
} // namespace Render
} // namespace Oryol