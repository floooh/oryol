//------------------------------------------------------------------------------
//  IMUI.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "IMUI.h"
#include "Core/Assertion.h"
#include "Core/Memory/Memory.h"

namespace Oryol {

IMUI::_state* IMUI::state = nullptr;

//------------------------------------------------------------------------------
void
IMUI::Setup(const IMUISetup& setup) {
    o_assert_dbg(!IsValid());
    state = Memory::New<_state>();
    state->setup = setup;
    state->imguiWrapper.Setup(setup);
}

//------------------------------------------------------------------------------
void
IMUI::Discard() {
    o_assert_dbg(IsValid());
    state->imguiWrapper.Discard();
    Memory::Delete(state);
    state = nullptr;
}

//------------------------------------------------------------------------------
void
IMUI::NewFrame() {
    o_assert_dbg(IsValid());
    state->imguiWrapper.NewFrame();
}

} // namespace Oryol

