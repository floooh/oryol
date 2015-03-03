//------------------------------------------------------------------------------
//  Synth.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Synth.h"

namespace Oryol {

Synth::_state* Synth::state = nullptr;

//------------------------------------------------------------------------------
void
Synth::Setup(const SynthSetup& setupAttrs) {
    o_assert(!IsValid());
    state = Memory::New<_state>();
    state->soundManager.Setup(setupAttrs);
}

//------------------------------------------------------------------------------
void
Synth::Discard() {
    o_assert(IsValid());
    state->soundManager.Discard();
    Memory::Delete(state);
    state = nullptr;
}

//------------------------------------------------------------------------------
bool
Synth::IsValid() {
    return nullptr != state;
}

//------------------------------------------------------------------------------
void
Synth::Update() {
    o_assert_dbg(IsValid());
    state->soundManager.Update();
}

//------------------------------------------------------------------------------
void
Synth::AddOp(int32 voice, int32 track, const SynthOp& op, int32 timeOffset) {
    o_assert_dbg(IsValid());
    state->soundManager.AddOp(voice, track, op, timeOffset);
}

} // namespace Oryol
