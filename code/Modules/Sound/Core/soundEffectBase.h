#pragma once
//------------------------------------------------------------------------------
/**
    @class soundEffectBase
    @ingroup _priv
    @brief base class for sound effect resource
*/
#include "Resource/Core/resourceBase.h"
#include "Sound/Core/SoundEffectSetup.h"

namespace Oryol {
namespace _priv {

class soundEffectBase : public resourceBase<SoundEffectSetup> {
public:
    /// clear the object
    void Clear();
};

} // namespace _priv
} // namespace Oryol