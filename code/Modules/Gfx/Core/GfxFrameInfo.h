#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::GfxFrameInfo
    @brief per-frame stats of the Gfx module
*/
#include "Core/Types.h"

namespace Oryol {

struct GfxFrameInfo {
    int32 NumApplyRenderTarget = 0;
    int32 NumApplyViewPort = 0;
    int32 NumApplyScissorRect = 0;
    int32 NumApplyDrawState = 0;
    int32 NumApplyUniformBlock = 0;
    int32 NumUpdateVertices = 0;
    int32 NumUpdateIndices = 0;
    int32 NumUpdateTextures = 0;
    int32 NumDraw = 0;
    int32 NumDrawInstanced = 0;
};

} // namespace Oryol
