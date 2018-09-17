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
    DbgDesc& NumTextColumns(int n)  { numTextColumns = n; return *this; }
    int NumTextColumns() const      { return numTextColumns; }
    /// number of text rows on screen
    DbgDesc& NumTextRows(int n)     { numTextRows = n; return *this; }
    int NumTextRows() const         { return numTextRows; }
    /// initial text scale, in x dimension
    DbgDesc& TextScaleX(float s)    { textScaleX = s; return *this; }
    float TextScaleX() const        { return textScaleX; }
    /// initial text scale, in y dimension
    DbgDesc& TextScaleY(float s)    { textScaleY = s; return *this; }
    float TextScaleY() const        { return textScaleY; }
    /// color pixel format of render pass to render text to
    DbgDesc& ColorFormat(PixelFormat::Code fmt)     { colorFormat = fmt; return *this; }
    PixelFormat::Code ColorFormat() const           { return colorFormat; }
    /// depth pixel format of render pass to render text to
    DbgDesc& DepthFormat(PixelFormat::Code fmt)     { depthFormat = fmt; return *this; }
    PixelFormat::Code DepthFormat() const           { return depthFormat; }
    /// sample count of render pass to render to
    DbgDesc& SampleCount(int c)     { sampleCount = c; return *this; }
    int SampleCount() const         { return sampleCount; }


    int numTextColumns = 120;
    int numTextRows = 80;
    float textScaleX = 1.0f;
    float textScaleY = 1.0f;
    PixelFormat::Code colorFormat = PixelFormat::RGBA8;
    PixelFormat::Code depthFormat = PixelFormat::DEPTHSTENCIL;
    int sampleCount = 1;
};

} // namespace Oryol
