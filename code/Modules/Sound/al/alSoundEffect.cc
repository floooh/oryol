//------------------------------------------------------------------------------
//  alSoundEffect.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "alSoundEffect.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
alSoundEffect::alSoundEffect() :
alBuffer(0),
nextSourceIndex(0),
numSources(0) {
    // empty
}

//------------------------------------------------------------------------------
alSoundEffect::~alSoundEffect() {
    o_assert_dbg(0 == this->alBuffer);
    for (int i = 0; i < SoundEffectSetup::MaxNumVoices; i++) {
        o_assert_dbg(0 == this->alSources[i]);
    }
}

//------------------------------------------------------------------------------
void
alSoundEffect::Clear() {
    this->alBuffer = 0;
    this->nextSourceIndex = 0;
    this->numSources = 0;
    this->alSources.Fill(0);
    soundEffectBase::Clear();
}

} // namespace _priv
} // namespace Oryol