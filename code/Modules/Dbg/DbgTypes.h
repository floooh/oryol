#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::DbgSetup
    @brief setup parameters for the Dbg module
*/
#include "Core/Types.h"
#include "Gfx/GfxTypes.h"

namespace Oryol {

class DbgSetup {
public:
    /// number of text columns on whole screen
    int NumTextColumns = 120;
    /// number of text lines on whole screen
    int NumTextRows = 80;
    /// initial text scale, x dimension
    float TextScaleX = 1.0f;
    /// initial text scale, y dimension
    float TextScaleY = 1.0f;
    /// color pixel format of render pass to render text to
    PixelFormat::Code ColorFormat = PixelFormat::RGBA8;
    /// depth pixel format of render pass to render text to
    PixelFormat::Code DepthFormat = PixelFormat::DEPTHSTENCIL;
    /// multi-sample count of render pass to render text to
    int SampleCount = 1;
};

} // namespace Oryol
