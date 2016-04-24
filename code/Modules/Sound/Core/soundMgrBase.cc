//------------------------------------------------------------------------------
//  soundMgrBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "soundMgrBase.h"
#include "Sound/Core/soundEffectPool.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
soundMgrBase::soundMgrBase() :
valid(false),
effectPool(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
soundMgrBase::~soundMgrBase() {
    o_assert_dbg(!this->valid);
    o_assert_dbg(nullptr == this->effectPool);
}

//------------------------------------------------------------------------------
void
soundMgrBase::setup(const SoundSetup& setup, soundEffectPool* sndEffectPool) {
    o_assert_dbg(!this->valid);
    o_assert_dbg(nullptr == this->effectPool);
    o_assert_dbg(nullptr != sndEffectPool);

    this->valid = true;
    this->effectPool = sndEffectPool;
}

//------------------------------------------------------------------------------
void
soundMgrBase::discard() {
    o_assert_dbg(this->valid);
    this->valid = false;
    this->effectPool = nullptr;
}

//------------------------------------------------------------------------------
bool
soundMgrBase::isValid() const {
    return this->valid;
}

//------------------------------------------------------------------------------
int
soundMgrBase::play(soundEffect* /*effect*/, int /*loopCount*/, float /*pitch*/, float /*volume*/) {
    return -1;
}

//------------------------------------------------------------------------------
void
soundMgrBase::stop(soundEffect* /*effect*/, int /*voice*/) {
    // empty
}

//------------------------------------------------------------------------------
void
soundMgrBase::setPitch(soundEffect* /*effect*/, int /*voice*/, float /*pitch*/) {
    // empty
}

//------------------------------------------------------------------------------
void
soundMgrBase::setVolume(soundEffect* /*effect*/, int /*voice*/, float /*volume*/) {
    // empty
}

} // namespace _priv
} // namespace Oryol