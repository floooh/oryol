#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::SoundEffectSetup
    @ingroup Sound
    @brief setup object for a canned sound effect
*/
#include "Core/Types.h"
#include "Resource/Locator.h"

namespace Oryol {

class SoundEffectSetup {
public:
    /// resource locator
    class Locator Locator;
};

} // namespace Oryol