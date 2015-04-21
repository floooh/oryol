//------------------------------------------------------------------------------
//  IMUI.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "IMUI.h"
#include "Core/Assertion.h"
#include "Core/Memory/Memory.h"

namespace Oryol {

//------------------------------------------------------------------------------
void
IMUI::Setup(const IMUISetup& setup) {
    o_assert_dbg(!IsValid());
    state = Memory::New<_state>();
    state->setup = setup;
}

//------------------------------------------------------------------------------
void
IMUI::Discard() {
    o_assert_dbg(!IsValid());
    Memory::Delete(state);
    state = nullptr;
}

} // namespace Oryol

