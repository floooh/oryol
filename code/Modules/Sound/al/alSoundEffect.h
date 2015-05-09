#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::alSoundEffect
    @ingroup _priv
    @brief OpenAL sound effect implementation
*/
#include "Sound/Core/soundEffectBase.h"

namespace Oryol {
namespace _priv {

class alSoundEffect : public soundEffectBase {
public:
    /// clear the object
    void Clear();
};

} // namespace _priv
} // namespace Oryol