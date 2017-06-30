#pragma once
//------------------------------------------------------------------------------
/**
    @defgroup Dbg Dbg
    @brief debug visualization and services

    @class Oryol::Dbg
    @ingroup Dbg
    @brief Dbg module facade
*/
#include "Core/Types.h"
#include "Dbg/DbgTypes.h"

namespace Oryol {
    
class Dbg {
public:
    /// setup the Debug module
    static void Setup(const DbgSetup& setup=DbgSetup());
    /// discard the Debug module
    static void Discard();
    /// return true if Debug module is valid
    static bool IsValid();
    
    /// set global text scale
    static void TextScale(float x, float y);
    /// add debug text (7-bit ASCII)
    static void Print(const char* text);
    /// add debug text (7-bit ASCII only)
    static void PrintF(const char* text, ...) __attribute__((format(printf, 1, 2)));
    /// add cursor positioning tag
    static void CursorPos(uint8_t x, uint8_t y);
    /// add color tag
    static void TextColor(float r, float g, float b, float a);
    /// add a color tag as float[4] array
    static void TextColor(const float (&c)[4]);
    /// draw the debug text buffer (call one per frame)
    static void DrawTextBuffer();
};
    
} // namespace Oryol