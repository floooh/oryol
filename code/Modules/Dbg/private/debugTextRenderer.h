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
#include "Dbg/DbgTypes.h"
#include <cstdarg>

namespace Oryol {
namespace _priv {

class debugTextRenderer {
public:
    ~debugTextRenderer();

    /// setup the text renderer
    void setup(const DbgSetup& setup);
    /// discard the text renderer
    void discard();
    /// return true if the text renderer has been setup
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
    /// setup the font texture
    void setupFontTexture();
    /// setup the text dynamic mesh
    void setupMesh();
    /// setup the text pipeline state object (happens deferred)
    void  setupPipeline();
    /// convert the provides string object into vertices, and return number of vertices
    void convertStringToVertices(const String& str);
    /// write one glyph vertex, returns next vertex index
    void addVertex(uint8_t x, uint8_t y, uint8_t u, uint8_t v, uint32_t rgba);
    
    static const int TabWidth = 8; // must be 2^n

    bool valid = false;
    VertexLayout vertexLayout;
    DrawState drawState;
    StringBuilder stringBuilder;
    ResourceLabel resourceLabel;
    int numColumns = 0;
    int numRows = 0;
    int maxNumChars = 0;
    int maxNumVertices = 0;
    struct Vertex {
        float x, y, u, v;
        uint32_t color;
    };
    int curNumVertices = 0;
    Vertex* vertexData = nullptr;
};

} // namespace _priv
} // namespace Oryol
