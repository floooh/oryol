#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::alSoundEffect
    @ingroup _priv
    @brief OpenAL sound effect implementation
*/
#include "Sound/Core/soundEffectBase.h"
#include "Sound/al/sound_al.h"
#include "Core/Containers/StaticArray.h"

namespace Oryol {
namespace _priv {

class alSoundEffect : public soundEffectBase {
public:
    /// constructor
    alSoundEffect();
    /// destructor
    ~alSoundEffect();

    static const int32 NumSources = 16;

    /// the alBuffer object
    ALuint alBuffer;
    /// the next source index to grab for playback
    int32 nextSourceIndex;
    /// alSource objects, used round-robin for playback
    StaticArray<ALuint, NumSources> alSources;

    /// clear the object
    void Clear();
};

} // namespace _priv
} // namespace Oryol