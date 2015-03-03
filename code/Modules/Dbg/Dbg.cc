//------------------------------------------------------------------------------
//  Dbg
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Dbg.h"
#include "Core/Trace.h"
#include <cstdarg>

namespace Oryol {

struct Dbg::_state* Dbg::state = nullptr;

//------------------------------------------------------------------------------
void
Dbg::Setup() {
    o_assert(!IsValid());
    state = Memory::New<_state>();
}

//------------------------------------------------------------------------------
void
Dbg::Discard() {
    o_assert(IsValid());
    Memory::Delete(state);
    state = nullptr;
}

//------------------------------------------------------------------------------
bool
Dbg::IsValid() {
    return nullptr != state;
}

//------------------------------------------------------------------------------
void
Dbg::SetTextScale(const glm::vec2& s) {
    o_assert_dbg(IsValid());
    state->debugTextRenderer.setTextScale(s);
}

//------------------------------------------------------------------------------
const glm::vec2&
Dbg::GetTextScale() {
    o_assert_dbg(IsValid());
    return state->debugTextRenderer.getTextScale();
}

//------------------------------------------------------------------------------
void
Dbg::Print(const char* text) {
    o_assert_dbg(IsValid());
    state->debugTextRenderer.print(text);
}

//------------------------------------------------------------------------------
void
Dbg::PrintF(const char* text, ...) {
    o_assert_dbg(IsValid());
    std::va_list args;
    va_start(args, text);
    state->debugTextRenderer.printf(text, args);
    va_end(args);
}

//------------------------------------------------------------------------------
void
Dbg::CursorPos(uint8 x, uint8 y) {
    o_assert_dbg(IsValid());
    state->debugTextRenderer.cursorPos(x, y);
}

//------------------------------------------------------------------------------
void
Dbg::TextColor(const glm::vec4& c) {
    o_assert_dbg(IsValid());
    state->debugTextRenderer.textColor(c);
}

//------------------------------------------------------------------------------
void
Dbg::DrawTextBuffer() {
    o_trace_scoped(Dbg_DrawTextBuffer);
    o_assert_dbg(IsValid());
    state->debugTextRenderer.drawTextBuffer();
}

} // namespace Oryol
