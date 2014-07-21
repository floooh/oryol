#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::DisplayAttrs
    @brief actual display attributes
    
    Holds the properties of the current display, these can be 
    different from the display setup parameters.
*/
#include "Core/Types.h"
#include "Core/String/String.h"
#include "Render/Core/Enums.h"

namespace Oryol {
namespace Render {

class DisplayAttrs {
public:
    int32 WindowWidth{0};
    int32 WindowHeight{0};
    int32 WindowPosX{0};
    int32 WindowPosY{0};
    int32 FramebufferWidth{0};
    int32 FramebufferHeight{0};
    PixelFormat::Code ColorPixelFormat{PixelFormat::InvalidPixelFormat};
    PixelFormat::Code DepthPixelFormat{PixelFormat::InvalidPixelFormat};
    bool IsFullscreen{false};
    int32 SwapInterval{1};
    Core::String WindowTitle;
};
} // namespace Render
} // namespace Oryol
 