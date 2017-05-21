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
debugTextRenderer::debugTextRenderer() {
    // NOTE: text rendering will be setup lazily when the text rendering
    // method is called first
    this->stringBuilder.Reserve(MaxNumChars * 2);
}

//------------------------------------------------------------------------------
debugTextRenderer::~debugTextRenderer() {
    if (this->valid) {
        this->discard();
    }
}

//------------------------------------------------------------------------------
void
debugTextRenderer::setup() {
    o_assert(!this->valid);
    this->resourceLabel = Gfx::PushResourceLabel();
    this->setupTextMesh();
    this->setupTextPipeline();
    this->setupFontTexture();
    Gfx::PopResourceLabel();
    this->valid = true;
}

//------------------------------------------------------------------------------
void
debugTextRenderer::discard() {
    o_assert(this->valid);
    this->valid = false;
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
    this->stringBuilder.Append((char) 0x1B);   // start ESC control sequence
    this->stringBuilder.Append((char) 0x01);   // set cursor
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
debugTextRenderer::drawTextBuffer() {
    
    // one-time setup
    if (!this->valid) {
        this->setup();
    }
    
    // get the currently accumulated string
    String str;
    {
        SCOPED_LOCK;
        str = this->stringBuilder.GetString();
        this->stringBuilder.Clear();
    }
    
    // convert string into vertices
    int numVertices = this->convertStringToVertices(str);

    // draw the vertices
    if (numVertices > 0) {
        // compute the size factor for one 8x8 glyph on screen
        // FIXME: this would be wrong if rendering to a render target which
        // isn't the same size as the back buffer, there's no method yet
        // to query the current render target width/height
        DbgTextShader::vsParams vsParams;
        const float w = 8.0f / Gfx::PassAttrs().FramebufferWidth;   // glyph is 8 pixels wide
        const float h = 8.0f / Gfx::PassAttrs().FramebufferHeight;  // glyph is 8 pixel tall
        vsParams.glyphSize = glm::vec2(w * this->textScaleX * 2.0f, h * this->textScaleY * 2.0f);

        Gfx::UpdateVertices(this->drawState.Mesh[0], this->vertexData, numVertices * this->vertexLayout.ByteSize());
        Gfx::ApplyDrawState(this->drawState);
        Gfx::ApplyUniformBlock(vsParams);
        Gfx::Draw(PrimitiveGroup(0, numVertices));
    }
}

//------------------------------------------------------------------------------
void
debugTextRenderer::setupFontTexture() {

    // convert the KC85/4 font into 8bpp image data
    const int numChars = 128;
    const int charWidth = 8;
    const int charHeight = 8;
    const int imgWidth = numChars * charWidth;
    const int imgHeight = charHeight;
    const int bytesPerChar = charWidth * charHeight;
    const int imgDataSize = numChars * bytesPerChar;
    o_assert((imgWidth * imgHeight) == imgDataSize);
    
    // setup a memory buffer and write font image data to it
    Buffer data;
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
    auto texSetup = TextureSetup::FromPixelData2D(imgWidth, imgHeight, 1, PixelFormat::L8);
    texSetup.Sampler.MinFilter = TextureFilterMode::Nearest;
    texSetup.Sampler.MagFilter = TextureFilterMode::Nearest;
    texSetup.Sampler.WrapU = TextureWrapMode::ClampToEdge;
    texSetup.Sampler.WrapV = TextureWrapMode::ClampToEdge;
    texSetup.ImageData.Sizes[0][0] = imgDataSize;
    Id tex = Gfx::CreateResource(texSetup, data);
    o_assert_dbg(tex.IsValid());
    o_assert_dbg(Gfx::QueryResourceInfo(tex).State == ResourceState::Valid);
    this->drawState.FSTexture[DbgTextShader::tex] = tex;
}

//------------------------------------------------------------------------------
void
debugTextRenderer::setupTextMesh() {
    o_assert(this->vertexLayout.Empty());
    
    // setup an empty mesh, only vertices
    int maxNumVerts = MaxNumChars * 6;
    this->vertexLayout = {
        { VertexAttr::Position, VertexFormat::Float4 },
        { VertexAttr::Color0, VertexFormat::UByte4N }
    };
    o_assert(sizeof(this->vertexData) == maxNumVerts * this->vertexLayout.ByteSize());
    MeshSetup setup = MeshSetup::Empty(maxNumVerts, Usage::Stream);
    setup.Layout = this->vertexLayout;
    this->drawState.Mesh[0] = Gfx::CreateResource(setup);
    o_assert(this->drawState.Mesh[0].IsValid());
    o_assert(Gfx::QueryResourceInfo(this->drawState.Mesh[0]).State == ResourceState::Valid);
}

//------------------------------------------------------------------------------
void
debugTextRenderer::setupTextPipeline() {
    // finally create pipeline object
    Id shd = Gfx::CreateResource(DbgTextShader::Setup());
    auto ps = PipelineSetup::FromLayoutAndShader(this->vertexLayout, shd);
    ps.DepthStencilState.DepthWriteEnabled = false;
    ps.DepthStencilState.DepthCmpFunc = CompareFunc::Always;
    ps.BlendState.BlendEnabled = true;
    ps.BlendState.SrcFactorRGB = BlendFactor::SrcAlpha;
    ps.BlendState.DstFactorRGB = BlendFactor::OneMinusSrcAlpha;
    ps.BlendState.ColorWriteMask = PixelChannel::RGB;
    // NOTE: this is a bit naughty, we actually want 'dbg render contexts'
    // for different render targets and quickly select them before
    // text rendering
    ps.BlendState.ColorFormat = Gfx::PassAttrs().ColorPixelFormat;
    ps.BlendState.DepthFormat = Gfx::PassAttrs().DepthPixelFormat;
    ps.RasterizerState.SampleCount = Gfx::PassAttrs().SampleCount;
    this->drawState.Pipeline = Gfx::CreateResource(ps);
}

//------------------------------------------------------------------------------
int
debugTextRenderer::writeVertex(int index, uint8_t x, uint8_t y, uint8_t u, uint8_t v, uint32_t rgba) {
    this->vertexData[index].x = (float) x;
    this->vertexData[index].y = (float) y;
    this->vertexData[index].u = (float) u;
    this->vertexData[index].v = (float) v;
    this->vertexData[index].color = rgba;
    return index + 1;
}

//------------------------------------------------------------------------------
int
debugTextRenderer::convertStringToVertices(const String& str) {

    int cursorX = 0;
    int cursorY = 0;
    const int cursorMaxX = MaxNumColumns - 1;
    const int cursorMaxY = MaxNumLines - 1;
    int vIndex = 0;
    uint32_t rgba = 0xFF00FFFF;
    
    const int numChars = str.Length() > MaxNumChars ? MaxNumChars : str.Length();
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
                        o_assert((charIndex + 1) < numChars);
                        char escCode = ptr[charIndex + 1];
                        if (escCode == 1) {
                            // reposition cursor
                            o_assert((charIndex + 3) < numChars);
                            cursorX = ptr[charIndex + 2];
                            cursorY = ptr[charIndex + 3];
                            charIndex += 3;
                        }
                        else if (escCode == 2) {
                            // change color
                            o_assert((charIndex + 5) < numChars);
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
            if ((vIndex < (MaxNumVertices - 6)) && (cursorX <= cursorMaxX)) {
                // renderable character, only consider 7 bit (codes > 127 can be
                // used to render control-code characters)
                c &= 0x7F;
                
                // write 6 vertices
                vIndex = this->writeVertex(vIndex, cursorX, cursorY, c, 0, rgba);
                vIndex = this->writeVertex(vIndex, cursorX+1, cursorY, c+1, 0, rgba);
                vIndex = this->writeVertex(vIndex, cursorX+1, cursorY+1, c+1, 1, rgba);
                vIndex = this->writeVertex(vIndex, cursorX, cursorY, c, 0, rgba);
                vIndex = this->writeVertex(vIndex, cursorX+1, cursorY+1, c+1, 1, rgba);
                vIndex = this->writeVertex(vIndex, cursorX, cursorY+1, c, 1, rgba);
                
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

} // namespace _priv
} // namespace Oryol
