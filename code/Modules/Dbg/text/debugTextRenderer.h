#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::debugTextRenderer
    @ingroup _priv
    @brief minimalistic 7-bit-ASCII text renderer
*/
#include "Core/Types.h"
#include "Resource/ResourceLabel.h"
#include "Core/String/StringBuilder.h"
#include "Gfx/Gfx.h"
#include <cstdarg>

namespace Oryol {
namespace _priv {

class debugTextRenderer {
public:
    debugTextRenderer();
    ~debugTextRenderer();

    void discard();
    bool isValid() const;

    float textScaleX = 1.0f, textScaleY = 1.0f;

    /// draw text (only adds to internal buffer)
    void print(const char* text);
    /// draw text (only adds to internal buffer)
    void printf(const char* text, std::va_list args);
    /// reposition the cursor
    void cursorPos(uint8_t x, uint8_t y);
    /// change text color
    void textColor(float r, float g, float b, float a);
    /// draw the accumulated text
    void drawTextBuffer();
    
private:
    /// setup the text renderer (will be called as needed)
    void setup();

    /// setup the font texture
    void setupFontTexture();
    /// setup the text dynamic mesh
    void setupTextMesh();
    /// setup the text pipeline state object
    void  setupTextPipeline();
    /// convert the provides string object into vertices, and return number of vertices
    int convertStringToVertices(const String& str);
    /// write one glyph vertex, returns next vertex index
    int writeVertex(int vertexIndex, uint8_t x, uint8_t y, uint8_t u, uint8_t v, uint32_t rgba);
    
    static const int MaxNumColumns = 120;
    static const int MaxNumLines = 80;
    static const int MaxNumChars = MaxNumColumns * MaxNumLines;
    static const int MaxNumVertices = MaxNumChars * 6;
    static const int TabWidth = 8; // must be 2^n
    
    VertexLayout vertexLayout;
    DrawState drawState;
    StringBuilder stringBuilder;
    bool valid = false;
    ResourceLabel resourceLabel;
    
    struct Vertex {
        float x, y, u, v;
        uint32_t color;
    };
    struct Vertex vertexData[MaxNumVertices];
};

} // namespace _priv
} // namespace Oryol
