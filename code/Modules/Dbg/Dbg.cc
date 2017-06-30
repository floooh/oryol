//------------------------------------------------------------------------------
//  Dbg
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Dbg.h"
#include "Core/Trace.h"
#include "Dbg/private/debugTextRenderer.h"
#include <cstdarg>

namespace Oryol {

namespace {
    struct _state {
        class _priv::debugTextRenderer debugTextRenderer;
    };
    _state* state = nullptr;
}

//------------------------------------------------------------------------------
void
Dbg::Setup(const DbgSetup& setup) {
    o_assert(!IsValid());
    state = Memory::New<_state>();
    state->debugTextRenderer.setup(setup);
}

//------------------------------------------------------------------------------
void
Dbg::Discard() {
    o_assert(IsValid());
    if (state->debugTextRenderer.isValid()) {
        state->debugTextRenderer.discard();
    }
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
Dbg::TextScale(float x, float y) {
    o_assert_dbg(IsValid());
    state->debugTextRenderer.textScaleX = x;
    state->debugTextRenderer.textScaleY = y;
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
Dbg::CursorPos(uint8_t x, uint8_t y) {
    o_assert_dbg(IsValid());
    state->debugTextRenderer.cursorPos(x, y);
}

//------------------------------------------------------------------------------
void
Dbg::TextColor(float r, float g, float b, float a) {
    o_assert_dbg(IsValid());
    state->debugTextRenderer.textColor(r, g, b, a);
}

//------------------------------------------------------------------------------
void
Dbg::TextColor(const float (&c)[4]) {
    o_assert_dbg(IsValid());
    state->debugTextRenderer.textColor(c[0], c[1], c[2], c[3]);
}
//------------------------------------------------------------------------------
void
Dbg::DrawTextBuffer() {
    o_trace_scoped(Dbg_DrawTextBuffer);
    o_assert_dbg(IsValid());
    state->debugTextRenderer.drawTextBuffer();
}

} // namespace Oryol
