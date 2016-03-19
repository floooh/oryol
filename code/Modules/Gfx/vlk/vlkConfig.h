#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::vlkConfig
    @brief Vulkan config constants
*/
#include "Core/Types.h"
#include "Gfx/Core/GfxConfig.h"

namespace Oryol {
namespace _priv {

class vlkConfig {
public:
    /// the number of frames that can be in-flight
    static const int NumFrames = 2;    
};

} // namespace _priv
} // namespace Oryol
