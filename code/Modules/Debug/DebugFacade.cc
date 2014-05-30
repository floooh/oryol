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
DebugFacade::SetTextColor(const glm::vec4& c) {
    this->debugTextRenderer.setTextColor(c);
}

//------------------------------------------------------------------------------
const glm::vec4&
DebugFacade::GetTextColor() const {
    return this->debugTextRenderer.getTextColor();
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
DebugFacade::Text(const char* text, ...) {
    std::va_list args;
    va_start(args, text);
    this->debugTextRenderer.text(text, args);
    va_end(args);
}

//------------------------------------------------------------------------------
void
DebugFacade::DrawTextBuffer() {
    this->debugTextRenderer.drawTextBuffer();
}

} // namespace Debug
} // namespace Oryol