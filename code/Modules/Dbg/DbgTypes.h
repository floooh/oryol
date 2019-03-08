#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::DbgSetup
    @brief setup parameters for the Dbg module
*/
#include "Core/Types.h"
#include "Gfx/GfxTypes.h"

namespace Oryol {

class DbgDesc {
public:
    /// number of text columns on screen
    int NumTextColumns = 120;
    /// number of text rows on screen
    int NumTextRows = 80;
    /// initial text scale, in x dimension
    float TextScaleX = 1.0f;
    /// initial text scale, in y dimension
    float TextScaleY = 1.0f;
    /// color pixel format of render pass to render text to
    PixelFormat::Code ColorFormat = PixelFormat::RGBA8;
    /// depth pixel format of render pass to render text to
    PixelFormat::Code DepthFormat = PixelFormat::DEPTHSTENCIL;
    /// sample count of render pass to render to
    int SampleCount = 1;

    /// setters with chaining
    DbgDesc& SetNumTextColumns(int n)  { NumTextColumns = n; return *this; }
    DbgDesc& SetNumTextRows(int n)     { NumTextRows = n; return *this; }
    DbgDesc& SetTextScaleX(float s)    { TextScaleX = s; return *this; }
    DbgDesc& SetTextScaleY(float s)    { TextScaleY = s; return *this; }
    DbgDesc& SetColorFormat(PixelFormat::Code fmt)     { ColorFormat = fmt; return *this; }
    DbgDesc& SetDepthFormat(PixelFormat::Code fmt)     { DepthFormat = fmt; return *this; }
    DbgDesc& SetSampleCount(int c)     { SampleCount = c; return *this; }
};

} // namespace Oryol
