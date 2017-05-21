//------------------------------------------------------------------------------
//  Dbg
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Dbg.h"
#include "Core/Trace.h"
#include "Dbg/private/debugTextRenderer.h"
#include <cstdarg>

namespace Oryol {

struct _dbg_state {
    class _priv::debugTextRenderer debugTextRenderer;
};
_dbg_state* dbgState = nullptr;

//------------------------------------------------------------------------------
void
Dbg::Setup() {
    o_assert(!IsValid());
    dbgState = Memory::New<_dbg_state>();
}

//------------------------------------------------------------------------------
void
Dbg::Discard() {
    o_assert(IsValid());
    if (dbgState->debugTextRenderer.isValid()) {
        dbgState->debugTextRenderer.discard();
    }
    Memory::Delete(dbgState);
    dbgState = nullptr;
}

//------------------------------------------------------------------------------
bool
Dbg::IsValid() {
    return nullptr != dbgState;
}

//------------------------------------------------------------------------------
void
Dbg::TextScale(float x, float y) {
    o_assert_dbg(IsValid());
    dbgState->debugTextRenderer.textScaleX = x;
    dbgState->debugTextRenderer.textScaleY = y;
}

//------------------------------------------------------------------------------
void
Dbg::Print(const char* text) {
    o_assert_dbg(IsValid());
    dbgState->debugTextRenderer.print(text);
}

//------------------------------------------------------------------------------
void
Dbg::PrintF(const char* text, ...) {
    o_assert_dbg(IsValid());
    std::va_list args;
    va_start(args, text);
    dbgState->debugTextRenderer.printf(text, args);
    va_end(args);
}

//------------------------------------------------------------------------------
void
Dbg::CursorPos(uint8_t x, uint8_t y) {
    o_assert_dbg(IsValid());
    dbgState->debugTextRenderer.cursorPos(x, y);
}

//------------------------------------------------------------------------------
void
Dbg::TextColor(float r, float g, float b, float a) {
    o_assert_dbg(IsValid());
    dbgState->debugTextRenderer.textColor(r, g, b, a);
}

//------------------------------------------------------------------------------
void
Dbg::TextColor(const float (&c)[4]) {
    o_assert_dbg(IsValid());
    dbgState->debugTextRenderer.textColor(c[0], c[1], c[2], c[3]);
}
//------------------------------------------------------------------------------
void
Dbg::DrawTextBuffer() {
    o_trace_scoped(Dbg_DrawTextBuffer);
    o_assert_dbg(IsValid());
    dbgState->debugTextRenderer.drawTextBuffer();
}

} // namespace Oryol
