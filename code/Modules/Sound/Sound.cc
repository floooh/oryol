//------------------------------------------------------------------------------
//  Sound.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Sound.h"

namespace Oryol {

using namespace _priv;

Sound::_state* Sound::state = nullptr;

//------------------------------------------------------------------------------
void
Sound::Setup(const SoundSetup& setup) {
    o_assert_dbg(!IsValid());

    state = Memory::New<_state>();
    state->soundSetup = setup;
    state->soundMgr.setup(setup, &state->resourceContainer.effectPool);
    state->resourceContainer.setup(setup);
}

//------------------------------------------------------------------------------
void
Sound::Discard() {
    o_assert_dbg(IsValid());

    state->resourceContainer.Destroy(ResourceLabel::All);
    state->soundMgr.discard();
    state->resourceContainer.discard();
    Memory::Delete(state);
    state = nullptr;
}

//------------------------------------------------------------------------------
bool
Sound::IsValid() {
    return nullptr != state;
}

//------------------------------------------------------------------------------
Id
Sound::CreateResource(const SoundEffectSetup& setup) {
    o_assert_dbg(IsValid());
    return state->resourceContainer.Create(setup);
}

//------------------------------------------------------------------------------
Id
Sound::CreateResource(const SoundEffectSetup& setup, const Ptr<Stream>& stream) {
    o_assert_dbg(IsValid());
    stream->Open(OpenMode::ReadOnly);
    const void* data = stream->MapRead(nullptr);
    const int32 size = stream->Size();
    Id id = state->resourceContainer.Create(setup, data, size);
    stream->UnmapRead();
    stream->Close();
    return id;
}

//------------------------------------------------------------------------------
Id
Sound::CreateResource(const SetupAndStream<SoundEffectSetup>& setupAndStream) {
    o_assert_dbg(IsValid());
    return CreateResource(setupAndStream.Setup, setupAndStream.Stream);
}

//------------------------------------------------------------------------------
Id
Sound::CreateResource(const SoundEffectSetup& setup, const void* data, int32 size) {
    o_assert_dbg(IsValid());
    o_assert_dbg(nullptr != data);
    o_assert_dbg(size > 0);
    return state->resourceContainer.Create(setup, data, size);
}

//------------------------------------------------------------------------------
void
Sound::DestroyResources(ResourceLabel label) {
    o_assert_dbg(IsValid());
    return state->resourceContainer.Destroy(label);
}

//------------------------------------------------------------------------------
void
Sound::Play(Id resId, int32 loopCount, int32 freqShift) {
    o_assert_dbg(IsValid());
    soundEffect* sndEffect = state->resourceContainer.lookupSoundEffect(resId);
    if (sndEffect) {
        state->soundMgr.play(sndEffect, loopCount, freqShift);
    }
}

} // namespace Oryol
