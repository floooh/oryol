#pragma once
//------------------------------------------------------------------------------
/**
    @defgroup Dbg Dbg
    @brief debug visualization and services

    @class Oryol::Dbg
    @ingroup Dbg
    @brief Dbg module facade
*/
#include "Core/Config.h"
#include "Dbg/text/debugTextRenderer.h"
#include "glm/fwd.hpp"

namespace Oryol {
    
class Dbg {
public:
    /// setup the Debug module
    static void Setup();
    /// discard the Debug module
    static void Discard();
    /// return true if Debug module is valid
    static bool IsValid();
    
    /// set global text scale factor (default is 1.0f, 1.0f)
    static void SetTextScale(const glm::vec2& scale);
    /// get global text scale
    static const glm::vec2& GetTextScale();
    
    /// add debug text (7-bit ASCII)
    static void Print(const char* text);
    /// add debug text (7-bit ASCII only)
    static void PrintF(const char* text, ...) __attribute__((format(printf, 1, 2)));
    /// add cursor positioning tag
    static void CursorPos(uint8 x, uint8 y);
    /// add color tag
    static void TextColor(const glm::vec4& color);
    /// draw the debug text buffer (call one per frame)
    static void DrawTextBuffer();
    
private:
    struct _state {
        class _priv::debugTextRenderer debugTextRenderer;
    };
    static _state* state;
};
    
} // namespace Oryol