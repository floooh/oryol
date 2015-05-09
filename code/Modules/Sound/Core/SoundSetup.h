#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::SoundSetup
    @ingroup Sound
    @brief Sound module initialization params
*/
#include "Core/Types.h"

namespace Oryol {

class SoundSetup {
public:
    /// sound effect pool size
    int32 SoundEffectPoolSize = 128;
    /// initial resource label stack capacity
    int32 ResourceLabelStackCapacity = 256;
    /// initial resource registry capacity
    int32 ResourceRegistryCapacity = 256;
};

} // namespace Oryol