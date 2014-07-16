#pragma once
//------------------------------------------------------------------------------
/**
    @class Debug::DebugFacade
    @brief various runtime debugging and debug visualization services
*/
#include "Core/Config.h"
#include "Core/Singleton.h"
#include "Debug/text/debugTextRenderer.h"
#include "glm/fwd.hpp"

namespace Oryol {
namespace Debug {
    
class DebugFacade {
    OryolLocalSingletonDecl(DebugFacade);
public:
    /// constructor
    DebugFacade();
    /// destructor
    ~DebugFacade();
    
    /// set global text scale factor (default is 1.0f, 1.0f)
    void SetTextScale(const glm::vec2& scale);
    /// get global text scale
    const glm::vec2& GetTextScale() const;
    
    /// add debug text (7-bit ASCII)
    void Print(const char* text);
    /// add debug text (7-bit ASCII only)
    void PrintF(const char* text, ...) __attribute__((format(printf, 2, 3)));
    /// add cursor positioning tag
    void CursorPos(uint8 x, uint8 y);
    /// add color tag
    void TextColor(const glm::vec4& color);
    /// draw the debug text buffer (call one per frame)
    void DrawTextBuffer();
    
private:
    class debugTextRenderer debugTextRenderer;
};
    
} // namespace Debug
} // namespace Oryol