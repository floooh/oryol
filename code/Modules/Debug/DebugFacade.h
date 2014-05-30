#pragma once
//------------------------------------------------------------------------------
/**
    @class Debug::DebugFacade
    @brief various runtime debugging and debug visualization services
*/
#include "Core/Config.h"
#include "Core/Macros.h"
#include "Debug/debugTextRenderer.h"
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
    
    /// set text scale factor (default is 1.0f, 1.0f)
    void SetTextScale(const glm::vec2& scale);
    /// get text scale
    const glm::vec2& GetTextScale() const;
    /// set text color (default is yellow)
    void SetTextColor(const glm::vec4& color);
    /// get text color
    const glm::vec4& GetTextColor() const;
    
    /// add debug text (7-bit ASCII only)
    void Text(const char* text, ...) __attribute__((format(printf, 2, 3)));
    /// draw the debug text buffer (call one per frame)
    void DrawTextBuffer();
    
private:
    class debugTextRenderer debugTextRenderer;
};
    
} // namespace Debug
} // namespace Oryol