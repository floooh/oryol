#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlTypes
    @ingroup _priv
    @brief Oryol to Metal type mapping
    
    NOTE: only include this file from a Objective-C(++) file!
*/
#include "Gfx/Core/Enums.h"

namespace Oryol {
namespace _priv {

class mtlTypes {
public:
    /// convert render target pixel format
    static MTLPixelFormat asRenderTargetFormat(PixelFormat::Code fmt);
    /// convert usage to MTLResourceOptions bit mask
    static MTLResourceOptions asBufferResourceOptions(Usage::Code usage);
};

} // namespace _priv
} // namespace Oryol