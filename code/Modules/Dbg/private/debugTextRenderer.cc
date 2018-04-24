//------------------------------------------------------------------------------
//  debugTextRenderer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "debugTextRenderer.h"
#include "Gfx/Gfx.h"
#include "DebugShaders.h"

#if ORYOL_HAS_THREADS
#include <mutex>
static std::mutex lockMutex;
#define SCOPED_LOCK std::lock_guard<std::mutex> lock(lockMutex)
#else
#define SCOPED_LOCK
#endif

namespace Oryol {
namespace _priv {

extern const char *kc85_4_Font;

//------------------------------------------------------------------------------
debugTextRenderer::~debugTextRenderer() {
    if (this->valid) {
        this->discard();
    }
}

//------------------------------------------------------------------------------
void
debugTextRenderer::setup(const DbgDesc& desc) {
    o_assert_dbg(!this->valid);
    o_assert_dbg(nullptr == this->vertexData);

    this->numColumns = desc.numTextColumns;
    this->numRows = desc.numTextRows;
    this->maxNumChars = desc.numTextColumns * desc.numTextRows;
    this->textScaleX = desc.textScaleX;
    this->textScaleY = desc.textScaleY;
    this->maxNumVertices = this->maxNumChars * 6;
    this->stringBuilder.Reserve(this->maxNumChars * 2);
    this->curNumVertices = 0;
    this->vertexData = (Vertex*) Memory::Alloc(this->maxNumVertices * sizeof(Vertex));
    Gfx::PushResourceLabel();
    this->setupResources(desc);
    this->resourceLabel = Gfx::PopResourceLabel();
    this->valid = true;
}

//------------------------------------------------------------------------------
void
debugTextRenderer::discard() {
    o_assert_dbg(this->valid);
    o_assert_dbg(this->vertexData);
    this->valid = false;
    Memory::Free(this->vertexData);
    this->vertexData = nullptr;
    Gfx::DestroyResources(this->resourceLabel);
}

//------------------------------------------------------------------------------
bool
debugTextRenderer::isValid() const {
    return this->valid;
}

//------------------------------------------------------------------------------
void
debugTextRenderer::print(const char* txt) {
    SCOPED_LOCK;
    this->stringBuilder.Append(txt);
}

//------------------------------------------------------------------------------
void
debugTextRenderer::printf(const char* text, std::va_list args) {
    SCOPED_LOCK;
    this->stringBuilder.AppendFormatVAList(1024, text, args);
}

//------------------------------------------------------------------------------
void
debugTextRenderer::cursorPos(uint8_t x, uint8_t y) {
    SCOPED_LOCK;
    this->stringBuilder.Append(0x1B);   // start ESC control sequence
    this->stringBuilder.Append(0x01);   // set cursor
    this->stringBuilder.Append((char)x);
    this->stringBuilder.Append((char)y);
}

//------------------------------------------------------------------------------
void
debugTextRenderer::textColor(float r, float g, float b, float a) {
    SCOPED_LOCK;
    this->stringBuilder.Append(0x1B);   // start ESC control sequence
    this->stringBuilder.Append(0x02);   // set color
    this->stringBuilder.Append(uint8_t(r * 255.0f));
    this->stringBuilder.Append(uint8_t(g * 255.0f));
    this->stringBuilder.Append(uint8_t(b * 255.0f));
    this->stringBuilder.Append(uint8_t(a * 255.0f));
}

//------------------------------------------------------------------------------
void
debugTextRenderer::drawTextBuffer(int width, int height) {
    
    // get the currently accumulated string
    String str;
    {
        SCOPED_LOCK;
        str = this->stringBuilder.GetString();
        this->stringBuilder.Clear();
    }
    
    // convert string into vertices
    this->convertStringToVertices(str);

    // draw the vertices
    if (this->curNumVertices > 0) {
        // compute the size factor for one 8x8 glyph on screen
        // FIXME: this would be wrong if rendering to a render target which
        // isn't the same size as the back buffer, there's no method yet
        // to query the current render target width/height
        if (0 == width) {
            width = Gfx::DisplayAttrs().Width;
        }
        if (0 == height) {
            height = Gfx::DisplayAttrs().Height;
        }
        Gfx::UpdateBuffer(this->drawState.VertexBuffers[0], this->vertexData, this->curNumVertices * this->vertexLayout.ByteSize());
        Gfx::ApplyDrawState(this->drawState);
        DbgTextShader::vsParams vsParams;
        const float w = 8.0f / width;   // glyph is 8 pixels wide
        const float h = 8.0f / height;  // glyph is 8 pixel tall
        vsParams.glyphSize = glm::vec2(w * this->textScaleX * 2.0f, h * this->textScaleY * 2.0f);
        Gfx::ApplyUniformBlock(vsParams);
        Gfx::Draw(0, this->curNumVertices);
        this->curNumVertices = 0;
    }
}

//------------------------------------------------------------------------------
void
debugTextRenderer::setupResources(const DbgDesc& desc) {
    o_assert_dbg(this->vertexLayout.Empty());
    o_assert_dbg((this->maxNumVertices > 0) && (this->maxNumVertices == this->maxNumChars*6));
    
    // setup an empty mesh, only vertices
    this->vertexLayout = {
        { "position", VertexFormat::Float4 },
        { "color0", VertexFormat::UByte4N }
    };
    const int vbufSize = this->maxNumVertices * this->vertexLayout.ByteSize();
    this->drawState.VertexBuffers[0] = Gfx::CreateBuffer(BufferDesc()
        .Size(vbufSize)
        .Type(BufferType::VertexBuffer)
        .Usage(Usage::Stream));
    o_assert_dbg(this->drawState.VertexBuffers[0].IsValid());

    // create pipeline object
    this->drawState.Pipeline = Gfx::CreatePipeline(PipelineDesc()
        .Shader(Gfx::CreateShader(DbgTextShader::Desc()))
        .Layout(0, this->vertexLayout)
        .DepthWriteEnabled(false)
        .DepthCmpFunc(CompareFunc::Always)
        .BlendEnabled(true)
        .BlendSrcFactorRGB(BlendFactor::SrcAlpha)
        .BlendDstFactorRGB(BlendFactor::OneMinusSrcAlpha)
        .ColorWriteMask(PixelChannel::RGB)
        .ColorFormat(desc.colorFormat)
        .DepthFormat(desc.depthFormat)
        .SampleCount(desc.sampleCount));

    // convert the KC85/4 font into 8bpp image data
    const int numChars = 128;
    const int charWidth = 8;
    const int charHeight = 8;
    const int imgWidth = numChars * charWidth;
    const int imgHeight = charHeight;
    const int bytesPerChar = charWidth * charHeight;
    const int imgDataSize = numChars * bytesPerChar;
    o_assert_dbg((imgWidth * imgHeight) == imgDataSize);

    // setup a memory buffer and write font image data to it
    MemoryBuffer data;
    uint8_t* dstPtr = data.Add(imgDataSize);
    const char* srcPtr = kc85_4_Font;
    for (int charIndex = 0; charIndex < numChars; charIndex++) {
        int xOffset = charIndex * charWidth;
        for (int y = 0; y < charHeight; y++) {
            int yOffset = y * imgWidth;
            for (int x = 0; x < charWidth; x++) {
                char c = *srcPtr++;
                o_assert_dbg(c != 0);
                dstPtr[x + xOffset + yOffset] = (c == '-') ? 0 : 255;
            }
        }
    }

    // setup texture, pixel format is 8bpp uncompressed
    this->drawState.FSTexture[DbgTextShader::tex] = Gfx::CreateTexture(TextureDesc()
        .Type(TextureType::Texture2D)
        .Width(imgWidth)
        .Height(imgHeight)
        .Format(PixelFormat::L8)
        .MinFilter(TextureFilterMode::Nearest)
        .MagFilter(TextureFilterMode::Nearest)
        .WrapU(TextureWrapMode::ClampToEdge)
        .WrapV(TextureWrapMode::ClampToEdge)
        .MipContent(0, 0, data.Data())
        .MipSize(0, 0, data.Size()));
}

//------------------------------------------------------------------------------
void
debugTextRenderer::addVertex(uint8_t x, uint8_t y, uint8_t u, uint8_t v, uint32_t rgba) {
    o_assert_dbg(this->curNumVertices < this->maxNumVertices);
    auto& vtx = this->vertexData[this->curNumVertices++];
    vtx.x = (float) x;
    vtx.y = (float) y;
    vtx.u = (float) u;
    vtx.v = (float) v;
    vtx.color = rgba;
}

//------------------------------------------------------------------------------
void
debugTextRenderer::convertStringToVertices(const String& str) {

    int cursorX = 0;
    int cursorY = 0;
    const int cursorMaxX = this->numColumns - 1;
    const int cursorMaxY = this->numRows - 1;
    uint32_t rgba = 0xFF00FFFF;
    
    const int numChars = str.Length() > this->maxNumChars ? this->maxNumChars : str.Length();
    const char* ptr = str.AsCStr();
    for (int charIndex = 0; charIndex < numChars; charIndex++) {
        unsigned char c = (unsigned char) ptr[charIndex];
        
        // control character?
        if (c < 0x20) {
            switch (c) {
                case 0x08: // cursor left
                    cursorX = cursorX > 0 ? cursorX - 1 : 0; break;
                case 0x09: // tab
                    cursorX = (cursorX & ~(TabWidth-1)) + TabWidth;
                    if (cursorX > cursorMaxX) {
                        cursorX = cursorMaxX;
                    }
                    break;
                case 0x0A: // cursor down
                    cursorY = cursorY < cursorMaxY ? cursorY + 1 : cursorMaxY; break;
                case 0x0B: // cursor up
                    cursorY = cursorY > 0 ? cursorY - 1 : 0; break;
                case 0x0D: // line feed
                    cursorX = 0; break;
                case 0x10: // home
                    cursorX = 0; cursorY = 0; break;
                case 0x1B: // handle escape sequence (position cursor or change text color)
                    {
                        o_assert_dbg((charIndex + 1) < numChars);
                        char escCode = ptr[charIndex + 1];
                        if (escCode == 1) {
                            // reposition cursor
                            o_assert_dbg((charIndex + 3) < numChars);
                            cursorX = ptr[charIndex + 2];
                            cursorY = ptr[charIndex + 3];
                            charIndex += 3;
                        }
                        else if (escCode == 2) {
                            // change color
                            o_assert_dbg((charIndex + 5) < numChars);
                            uint8_t r = (uint8_t) ptr[charIndex + 2];
                            uint8_t g = (uint8_t) ptr[charIndex + 3];
                            uint8_t b = (uint8_t) ptr[charIndex + 4];
                            uint8_t a = (uint8_t) ptr[charIndex + 5];
                            charIndex += 5;
                            rgba = (a<<24) | (b << 16) | (g << 8) | (r);
                        }
                        else {
                            o_error("Invalid escape code '%d'\n", escCode);
                        }
                    }
                default: break;
            }
        }
        else {
            // still space in vertex buffer?
            if ((this->curNumVertices < (this->maxNumVertices - 6)) && (cursorX <= cursorMaxX)) {
                // renderable character, only consider 7 bit (codes > 127 can be
                // used to render control-code characters)
                c &= 0x7F;
                
                // write 6 vertices
                this->addVertex(cursorX, cursorY, c, 0, rgba);
                this->addVertex(cursorX+1, cursorY, c+1, 0, rgba);
                this->addVertex(cursorX+1, cursorY+1, c+1, 1, rgba);
                this->addVertex(cursorX, cursorY, c, 0, rgba);
                this->addVertex(cursorX+1, cursorY+1, c+1, 1, rgba);
                this->addVertex(cursorX, cursorY+1, c, 1, rgba);

                // advance horizontal cursor position
                cursorX++;
            }
            else {
                // vertex buffer overflow
                break;
            }
        }
    }
}

} // namespace _priv
} // namespace Oryol
