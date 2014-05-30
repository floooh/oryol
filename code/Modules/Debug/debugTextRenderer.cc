//------------------------------------------------------------------------------
//  debugTextRenderer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "debugTextRenderer.h"
#include "Render/RenderFacade.h"
#include "IO/MemoryStream.h"
#include "DebugShaders.h"

namespace Oryol {
namespace Debug {

using namespace Core;
using namespace IO;
using namespace Render;

extern const char *kc85_4_Font;

//------------------------------------------------------------------------------
debugTextRenderer::debugTextRenderer() :
textScale(1.0f, 1.0f),
textColor(1.0f, 1.0f, 0.0f, 1.0f),
valid(false) {
    // NOTE: text rendering will be setup lazily when the text rendering
    // method is called first
}

//------------------------------------------------------------------------------
debugTextRenderer::~debugTextRenderer() {
    if (this->valid) {
        this->discard();
    }
}

//------------------------------------------------------------------------------
void
debugTextRenderer::setTextScale(const glm::vec2& s) {
    this->textScale = s;
}

//------------------------------------------------------------------------------
const glm::vec2&
debugTextRenderer::getTextScale() const {
    return this->textScale;
}

//------------------------------------------------------------------------------
void
debugTextRenderer::setTextColor(const glm::vec4& c) {
    this->textColor = c;
}

//------------------------------------------------------------------------------
const glm::vec4&
debugTextRenderer::getTextColor() const {
    return this->textColor;
}

//------------------------------------------------------------------------------
void
debugTextRenderer::setup() {
    o_assert(!this->valid);
    RenderFacade* renderFacade = RenderFacade::Instance();
    this->setupFontTexture(renderFacade);
    this->setupTextMesh(renderFacade);
    this->setupTextShader(renderFacade);
    this->valid = true;
}

//------------------------------------------------------------------------------
void
debugTextRenderer::discard() {
    o_assert(this->valid);
    this->valid = false;
    RenderFacade* renderFacade = RenderFacade::Instance();
    renderFacade->DiscardResource(this->textShader);
    renderFacade->DiscardResource(this->textMesh);
    renderFacade->DiscardResource(this->fontTexture);
    this->textShader.Invalidate();
    this->textMesh.Invalidate();
    this->fontTexture.Invalidate();
}

//------------------------------------------------------------------------------
bool
debugTextRenderer::isValid() const {
    return this->valid;
}

//------------------------------------------------------------------------------
void
debugTextRenderer::text(const char* text, std::va_list args) {
    this->rwLock.LockWrite();
    if (!this->valid) {
        this->setup();
    }
    this->stringBuilder.Format(1024, text, args);
    this->rwLock.UnlockWrite();
}

//------------------------------------------------------------------------------
void
debugTextRenderer::drawTextBuffer() {
    
    // get the currently accumulated string
    this->rwLock.LockWrite();
    String str = this->stringBuilder.GetString();
    this->stringBuilder.Clear();
    this->rwLock.UnlockWrite();
    
    // convert string into vertices
    int32 numVertices = this->convertStringToVertices(str);

    // draw the vertices
    if (numVertices > 0) {
        RenderFacade* renderFacade = RenderFacade::Instance();
        
        // compute the size factor for one 8x8 glyph on screen
        // FIXME: this would be wrong if rendering to a render target which
        // isn't the same size as the back buffer, there's no method yet
        // to query the current render target width/height
        const DisplayAttrs& dispAttrs = renderFacade->GetDisplayAttrs();
        const float w = 8.0f / dispAttrs.GetFramebufferWidth();   // glyph is 8 pixels wide
        const float h = 8.0f / dispAttrs.GetFramebufferHeight();  // glyph is 8 pixel tall
        const glm::vec2 glyphSize = glm::vec2(w * 2.0f, h * 2.0f) * this->textScale;
    
        renderFacade->UpdateVertices(this->textMesh, numVertices * this->vertexLayout.GetByteSize(), this->vertexData);
        
        renderFacade->ApplyMesh(this->textMesh);
        renderFacade->ApplyProgram(this->textShader, 0);
        renderFacade->ApplyVariable(DebugShaders::TextShader::GlyphSize, glyphSize);
        renderFacade->ApplyVariable(DebugShaders::TextShader::TextColor, this->textColor);
        renderFacade->ApplyVariable(DebugShaders::TextShader::Texture, this->fontTexture);
        
        renderFacade->ApplyState(State::ColorMask, true, true, true, true);
        renderFacade->ApplyState(State::DepthMask, false);
        renderFacade->ApplyState(State::DepthTestEnabled, false);
        renderFacade->ApplyState(State::BlendEnabled, true);
        renderFacade->ApplyState(State::BlendFunc, State::SrcAlpha, State::InvSrcAlpha);
        renderFacade->Draw(PrimitiveGroup(PrimitiveType::Triangles, 0, numVertices));
        renderFacade->ApplyState(State::BlendEnabled, false);
    }
}

//------------------------------------------------------------------------------
void
debugTextRenderer::setupFontTexture(RenderFacade* renderFacade) {
    o_assert(nullptr != renderFacade);
    o_assert(!this->fontTexture.IsValid());
    
    // convert the KC85/4 font into 8bpp image data
    const int32 numChars = 128;
    const int32 charWidth = 8;
    const int32 charHeight = 8;
    const int32 imgWidth = numChars * charWidth;
    const int32 imgHeight = charHeight;
    const int32 bytesPerChar = charWidth * charHeight;
    const int32 imgDataSize = numChars * bytesPerChar;
    o_assert((imgWidth * imgHeight) == imgDataSize);
    
    // setup a memory buffer and write font image data to it
    auto data = MemoryStream::Create();
    data->Open(OpenMode::WriteOnly);
    uint8* dstPtr = data->MapWrite(imgDataSize);
    const char* srcPtr = kc85_4_Font;
    for (int32 charIndex = 0; charIndex < numChars; charIndex++) {
        int32 xOffset = charIndex * charWidth;
        for (int32 y = 0; y < charHeight; y++) {
            int32 yOffset = y * imgWidth;
            for (int32 x = 0; x < charWidth; x++) {
                char c = *srcPtr++;
                o_assert_dbg(c != 0);
                dstPtr[x + xOffset + yOffset] = (c == '-') ? 0 : 255;
            }
        }
    }
    data->UnmapWrite();
    data->Close();
    
    // setup texture, pixel format is 8bpp uncompressed
    TextureSetup setup = TextureSetup::FromPixelData("_kc854font", imgWidth, imgHeight, false, PixelFormat::L8);
    setup.SetMinFilter(TextureFilterMode::Nearest);
    setup.SetMagFilter(TextureFilterMode::Nearest);
    setup.SetWrapU(TextureWrapMode::ClampToEdge);
    setup.SetWrapV(TextureWrapMode::ClampToEdge);
    this->fontTexture = renderFacade->CreateResource(setup, data);
    o_assert(this->fontTexture.IsValid());
    o_assert(renderFacade->QueryResourceState(this->fontTexture) == Resource::State::Valid);
}

//------------------------------------------------------------------------------
void
debugTextRenderer::setupTextMesh(RenderFacade* renderFacade) {
    o_assert(nullptr != renderFacade);
    o_assert(!this->textMesh.IsValid());
    o_assert(this->vertexLayout.Empty());
    
    // setup an empty mesh, only vertices
    int32 maxNumVerts = MaxNumChars * 6;
    this->vertexLayout.Add(VertexAttr::Position, VertexFormat::Short2);
    this->vertexLayout.Add(VertexAttr::TexCoord0, VertexFormat::Short2);
    o_assert(sizeof(this->vertexData) == maxNumVerts * this->vertexLayout.GetByteSize());
    MeshSetup setup = MeshSetup::CreateEmpty("_dbgText", this->vertexLayout, maxNumVerts, Usage::DynamicStream);
    this->textMesh = renderFacade->CreateResource(setup);
    o_assert(this->textMesh.IsValid());
    o_assert(renderFacade->QueryResourceState(this->textMesh) == Resource::State::Valid);
}

//------------------------------------------------------------------------------
void
debugTextRenderer::setupTextShader(RenderFacade* renderFacade) {
    o_assert(nullptr != renderFacade);
    o_assert(!this->textShader.IsValid());
    this->textShader = renderFacade->CreateResource(DebugShaders::TextShader::CreateSetup());
}

//------------------------------------------------------------------------------
int32
debugTextRenderer::writeVertex(int32 index, short x, short y, short u, short v) {
    this->vertexData[index][0] = x;
    this->vertexData[index][1] = y;
    this->vertexData[index][2] = u;
    this->vertexData[index][3] = v;
    return index + 1;
}

//------------------------------------------------------------------------------
int32
debugTextRenderer::convertStringToVertices(const String& str) {

    int32 cursorX = 0;
    int32 cursorY = 0;
    const int32 cursorMaxX = MaxNumColumns - 1;
    const int32 cursorMaxY = MaxNumLines - 1;
    int32 vIndex = 0;
    
    const int32 numChars = str.Length() > MaxNumChars ? MaxNumChars : str.Length();
    const char* ptr = str.AsCStr();
    for (int32 charIndex = 0; charIndex < numChars; charIndex++) {
        char c = ptr[charIndex];
        
        // control character?
        if (c < 0x20) {
            switch (c) {
                case 0x08: // cursor left
                    cursorX = cursorX > 0 ? cursorX - 1 : 0; break;
                case 0x09: // cursor right
                    cursorX = cursorX < cursorMaxX ? cursorX + 1 : cursorMaxX; break;
                case 0x0A: // cursor down
                    cursorY = cursorY < cursorMaxY ? cursorY + 1 : cursorMaxY; break;
                case 0x0B: // cursor up
                    cursorY = cursorY > 0 ? cursorY - 1 : 0; break;
                case 0x0D: // line feed
                    cursorX = 0; break;
                case 0x10: // home
                    cursorX = 0; cursorY = 0; break;
                default: break;
            }
        }
        else {
            // still space in vertex buffer?
            if ((vIndex < (MaxNumVertices - 6)) && (cursorX <= cursorMaxX)) {
                // renderable character, only consider 7 bit (codes > 127 can be
                // used to render control-code characters)
                c &= 0x7F;
                
                // write 6 vertices
                vIndex = this->writeVertex(vIndex, cursorX, cursorY, c, 0);
                vIndex = this->writeVertex(vIndex, cursorX+1, cursorY, c+1, 0);
                vIndex = this->writeVertex(vIndex, cursorX+1, cursorY+1, c+1, 1);
                vIndex = this->writeVertex(vIndex, cursorX, cursorY, c, 0);
                vIndex = this->writeVertex(vIndex, cursorX+1, cursorY+1, c+1, 1);
                vIndex = this->writeVertex(vIndex, cursorX, cursorY+1, c, 1);
                
                // advance horizontal cursor position
                cursorX++;
            }
            else {
                // vertex buffer overflow
                break;
            }
        }
    }
    return vIndex;
}

} // namespace Debug
} // namespace Oryol