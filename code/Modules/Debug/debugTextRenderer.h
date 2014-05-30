#pragma once
//------------------------------------------------------------------------------
/**
    @class Debug::debugTextRenderer
    @brief minimalistic 7-bit-ASCII text renderer
*/
#include "Core/Types.h"
#include "Resource/Id.h"
#include "Core/String/StringBuilder.h"
#include "Core/Threading/RWLock.h"
#include "Render/Core/VertexLayout.h"
#include "glm/vec2.hpp"
#include "glm/vec4.hpp"
#include <cstdarg>

namespace Oryol {

namespace Render {
    class RenderFacade;
}

namespace Debug {

class debugTextRenderer {
public:
    /// constructor
    debugTextRenderer();
    /// destructor
    ~debugTextRenderer();

    /// discard the text renderer
    void discard();
    /// return true if the object has been setup
    bool isValid() const;
    
    /// set text scale
    void setTextScale(const glm::vec2& s);
    /// get text scale
    const glm::vec2& getTextScale() const;
    /// set text color
    void setTextColor(const glm::vec4& c);
    /// get text color
    const glm::vec4& getTextColor() const;
    
    /// draw text at position (only adds to list)
    void text(const char* text, std::va_list args);
    /// draw the accumulated text
    void drawTextBuffer();
    
private:
    /// setup the text renderer (will be called as needed)
    void setup();

    /// setup the font texture
    void setupFontTexture(Render::RenderFacade* renderFacade);
    /// setup the text dynamic mesh
    void setupTextMesh(Render::RenderFacade* renderFacade);
    /// setup the text shader
    void setupTextShader(Render::RenderFacade* renderFacade);
    /// convert the provides string object into vertices, and return number of vertices
    int32 convertStringToVertices(const Core::String& str);
    /// write one glyph vertex, returns next vertex index
    int32 writeVertex(int32 vertexIndex, short x, short y, short u, short v);
    
    static const int32 MaxNumColumns = 120;
    static const int32 MaxNumLines = 80;
    static const int32 MaxNumChars = MaxNumColumns * MaxNumLines;
    static const int32 MaxNumVertices = MaxNumChars * 6;
    
    glm::vec2 textScale;
    glm::vec4 textColor;
    Render::VertexLayout vertexLayout;
    Core::RWLock rwLock;
    Resource::Id fontTexture;
    Resource::Id textMesh;
    Resource::Id textShader;
    Core::StringBuilder stringBuilder;
    bool valid;
    
    // at most 6 vertices per character, 4 shorts per vertex (2 pos, 2 uv)
    short vertexData[MaxNumVertices][4];
};

} // namespace Debug
} // namespace Oryol