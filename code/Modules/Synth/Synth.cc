//------------------------------------------------------------------------------
//  Synth.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Synth.h"
#include "Core/Trace.h"

namespace Oryol {

Synth::_state* Synth::state = nullptr;

//------------------------------------------------------------------------------
void
Synth::Setup(const class SynthSetup& setupAttrs) {
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
const class SynthSetup&
Synth::SynthSetup()
{
    o_assert(IsValid());
    return state->soundManager.setup;
}

//------------------------------------------------------------------------------
void
Synth::UpdateVolume(float32 vol) {
    o_assert_dbg(IsValid());
    state->soundManager.UpdateVolume(vol);
}

//------------------------------------------------------------------------------
void
Synth::Update() {
    o_assert_dbg(IsValid());
    o_trace_scoped(Synth_Update);
    state->soundManager.Update();
}

//------------------------------------------------------------------------------
void
Synth::AddOp(int32 voice, int32 track, const SynthOp& op, int32 timeOffset) {
    o_assert_dbg(IsValid());
    state->soundManager.AddOp(voice, track, op, timeOffset);
}

} // namespace Oryol
