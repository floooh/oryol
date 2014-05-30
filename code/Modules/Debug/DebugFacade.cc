//------------------------------------------------------------------------------
//  DebugFacade.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "DebugFacade.h"
#include <cstdarg>

namespace Oryol {
namespace Debug {

OryolLocalSingletonImpl(DebugFacade);
    
//------------------------------------------------------------------------------
DebugFacade::DebugFacade() {
    this->SingletonEnsureUnique();
}

//------------------------------------------------------------------------------
DebugFacade::~DebugFacade() {
    if (this->debugTextRenderer.isValid()) {
        this->debugTextRenderer.discard();
    }
}

//------------------------------------------------------------------------------
void
DebugFacade::SetTextScale(const glm::vec2& s) {
    this->debugTextRenderer.setTextScale(s);
}

//------------------------------------------------------------------------------
const glm::vec2&
DebugFacade::GetTextScale() const {
    return this->debugTextRenderer.getTextScale();
}

//------------------------------------------------------------------------------
void
DebugFacade::Print(const char* text) {
    this->debugTextRenderer.print(text);
}

//------------------------------------------------------------------------------
void
DebugFacade::PrintF(const char* text, ...) {
    std::va_list args;
    va_start(args, text);
    this->debugTextRenderer.printf(text, args);
    va_end(args);
}

//------------------------------------------------------------------------------
void
DebugFacade::CursorPos(uint8 x, uint8 y) {
    this->debugTextRenderer.cursorPos(x, y);
}

//------------------------------------------------------------------------------
void
DebugFacade::TextColor(const glm::vec4& c) {
    this->debugTextRenderer.textColor(c);
}

//------------------------------------------------------------------------------
void
DebugFacade::DrawTextBuffer() {
    this->debugTextRenderer.drawTextBuffer();
}

} // namespace Debug
} // namespace Oryol