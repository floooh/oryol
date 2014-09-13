//------------------------------------------------------------------------------
//  Debug.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Debug.h"
#include <cstdarg>

namespace Oryol {

struct Debug::_state* Debug::state = nullptr;

//------------------------------------------------------------------------------
void
Debug::Setup() {
    o_assert(!IsValid());
    state = new _state();
}

//------------------------------------------------------------------------------
void
Debug::Discard() {
    o_assert(IsValid());
    delete state;
    state = nullptr;
}

//------------------------------------------------------------------------------
bool
Debug::IsValid() {
    return nullptr != state;
}

//------------------------------------------------------------------------------
void
Debug::SetTextScale(const glm::vec2& s) {
    o_assert_dbg(IsValid());
    state->debugTextRenderer.setTextScale(s);
}

//------------------------------------------------------------------------------
const glm::vec2&
Debug::GetTextScale() {
    o_assert_dbg(IsValid());
    return state->debugTextRenderer.getTextScale();
}

//------------------------------------------------------------------------------
void
Debug::Print(const char* text) {
    o_assert_dbg(IsValid());
    state->debugTextRenderer.print(text);
}

//------------------------------------------------------------------------------
void
Debug::PrintF(const char* text, ...) {
    o_assert_dbg(IsValid());
    std::va_list args;
    va_start(args, text);
    state->debugTextRenderer.printf(text, args);
    va_end(args);
}

//------------------------------------------------------------------------------
void
Debug::CursorPos(uint8 x, uint8 y) {
    o_assert_dbg(IsValid());
    state->debugTextRenderer.cursorPos(x, y);
}

//------------------------------------------------------------------------------
void
Debug::TextColor(const glm::vec4& c) {
    o_assert_dbg(IsValid());
    state->debugTextRenderer.textColor(c);
}

//------------------------------------------------------------------------------
void
Debug::DrawTextBuffer() {
    o_assert_dbg(IsValid());
    state->debugTextRenderer.drawTextBuffer();
}

} // namespace Oryol
