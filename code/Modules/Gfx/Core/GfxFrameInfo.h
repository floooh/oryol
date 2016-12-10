#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::GfxFrameInfo
    @brief per-frame stats of the Gfx module
*/
#include "Core/Types.h"

namespace Oryol {

struct GfxFrameInfo {
    int NumPasses = 0;
    int NumApplyViewPort = 0;
    int NumApplyScissorRect = 0;
    int NumApplyDrawState = 0;
    int NumApplyUniformBlock = 0;
    int NumUpdateVertices = 0;
    int NumUpdateIndices = 0;
    int NumUpdateTextures = 0;
    int NumGenerateMipmaps = 0;
    int NumDraw = 0;
    int NumDrawInstanced = 0;
};

} // namespace Oryol
