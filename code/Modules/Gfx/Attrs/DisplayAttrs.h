#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::DisplayAttrs
    @ingroup Gfx
    @brief actual display attributes
    
    Holds the properties of the current display, these can be 
    different from the display setup parameters.
*/
#include "Core/Types.h"
#include "Core/String/String.h"
#include "Gfx/Core/Enums.h"

namespace Oryol {

struct DisplayAttrs {
    int32 WindowWidth{0};
    int32 WindowHeight{0};
    int32 WindowPosX{0};
    int32 WindowPosY{0};
    int32 FramebufferWidth{0};
    int32 FramebufferHeight{0};
    PixelFormat::Code ColorPixelFormat{PixelFormat::RGB8};
    PixelFormat::Code DepthPixelFormat{PixelFormat::D24S8};
    int32 Samples{0};
    bool Windowed{true};
    int32 SwapInterval{1};
    String WindowTitle;
};
} // namespace Oryol
 