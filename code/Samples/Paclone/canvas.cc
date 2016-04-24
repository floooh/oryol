//------------------------------------------------------------------------------
//  canvas.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "canvas.h"
#include "Gfx/Gfx.h"

using namespace Oryol;

namespace Paclone {

//------------------------------------------------------------------------------
canvas::canvas() :
isValid(false),
numTilesX(0),
numTilesY(0),
tileWidth(0),
tileHeight(0),
numVertices(0) {
    // empty
}

//------------------------------------------------------------------------------
void
canvas::Setup(const TextureSetup& rtSetup, int tilesX, int tilesY, int tileW, int tileH, int numSpr) {
    o_assert(!this->isValid);
    o_assert((tilesX > 0) && (tilesX <= MaxWidth) && (tilesY > 0) && (tilesY <= MaxHeight));
    o_assert(numSpr < MaxNumSprites);
    
    this->isValid = true;
    this->numTilesX = tilesX;
    this->numTilesY = tilesY;
    this->tileWidth = tileW;
    this->tileHeight = tileH;
    this->canvasWidth = this->numTilesX * this->tileWidth;
    this->canvasHeight = this->numTilesY * this->tileHeight;
    this->numSprites = numSpr;
    this->numVertices = (this->numTilesX * this->numTilesY + this->numSprites) * 6;
    
    // setup draw state with dynamic mesh
    auto meshSetup = MeshSetup::Empty(this->numVertices, Usage::Stream);
    meshSetup.Layout
        .Add(VertexAttr::Position, VertexFormat::Float2)
        .Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    meshSetup.AddPrimitiveGroup(PrimitiveGroup(0, this->numVertices));
    this->drawState.Mesh[0] = Gfx::CreateResource(meshSetup);
    Id shd = Gfx::CreateResource(CanvasShader::Setup());
    auto ps = PipelineSetup::FromLayoutAndShader(meshSetup.Layout, shd);
    ps.BlendState.BlendEnabled = true;
    ps.BlendState.SrcFactorRGB = BlendFactor::SrcAlpha;
    ps.BlendState.DstFactorRGB = BlendFactor::OneMinusSrcAlpha;
    ps.BlendState.ColorFormat = rtSetup.ColorFormat;
    ps.BlendState.DepthFormat = rtSetup.DepthFormat;
    ps.RasterizerState.CullFaceEnabled = false;
    this->drawState.Pipeline = Gfx::CreateResource(ps);
    
    // setup sprite texture
    auto texSetup = TextureSetup::FromPixelData(Sheet::Width, Sheet::Height, 1, TextureType::Texture2D, PixelFormat::RGBA8);
    texSetup.Sampler.MinFilter = TextureFilterMode::Nearest;
    texSetup.Sampler.MagFilter = TextureFilterMode::Nearest;
    texSetup.Sampler.WrapU = TextureWrapMode::ClampToEdge;
    texSetup.Sampler.WrapV = TextureWrapMode::ClampToEdge;
    texSetup.ImageData.Sizes[0][0] = Sheet::NumBytes;
    this->drawState.FSTexture[CanvasTextures::Texture] = Gfx::CreateResource(texSetup, Sheet::Pixels, Sheet::NumBytes);
    
    // initialize the tile map
    for (int y = 0; y < this->numTilesY; y++) {
        for (int x = 0; x < this->numTilesX; x++) {
            this->tiles[y * this->numTilesX + x] = Sheet::InvalidSprite;
        }
    }
    
    // clear the vertex buffer
    Memory::Clear(this->vertexBuffer, sizeof(this->vertexBuffer));
}

//------------------------------------------------------------------------------
void
canvas::Discard() {
    o_assert(this->isValid);
    this->isValid = false;
}

//------------------------------------------------------------------------------
bool
canvas::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
void
canvas::Render() {
    o_assert(this->isValid);
    int numBytes = 0;
    const void* data = this->updateVertices(numBytes);
    Gfx::UpdateVertices(this->drawState.Mesh[0], data, numBytes);
    Gfx::ApplyDrawState(this->drawState);
    Gfx::Draw(0);
}

//------------------------------------------------------------------------------
int
canvas::writeVertex(int index, float x, float y, float u, float v) {
    this->vertexBuffer[index].x = x;
    this->vertexBuffer[index].y = y;
    this->vertexBuffer[index].u = u;
    this->vertexBuffer[index].v = v;
    return index + 1;
}

//------------------------------------------------------------------------------
const void*
canvas::updateVertices(int& outNumBytes) {

    // write the tile map
    const float dx = 1.0f / this->numTilesX;
    const float dy = 1.0f / this->numTilesY;
    const float sheetWidth = float(Sheet::Width);
    const float sheetHeight = float(Sheet::Height);
    int vIndex = 0;
    for (int y = 0; y < this->numTilesY; y++) {
        const float y0 = y * dy;
        const float y1 = y0 + dy;
        for (int x = 0; x < this->numTilesX; x++) {
            
            // positions
            const float x0 = x * dx;
            const float x1 = x0 + dx;
            
            // uvs
            Sheet::SpriteId id = this->tiles[y * this->numTilesX + x];
            const auto& sprite = Sheet::Sprite[id];
            const float u0 = float(sprite.X) / sheetWidth;
            const float v0 = float(sprite.Y) / sheetHeight;
            const float u1 = u0 + float(sprite.W) / sheetWidth;
            const float v1 = v0 + float(sprite.H) / sheetHeight;
            
            // write vertex data (2 triangles)
            o_assert_dbg((vIndex + 6) <= this->numVertices);
            vIndex = this->writeVertex(vIndex, x0, y0, u0, v0);
            vIndex = this->writeVertex(vIndex, x1, y0, u1, v0);
            vIndex = this->writeVertex(vIndex, x1, y1, u1, v1);
            vIndex = this->writeVertex(vIndex, x0, y0, u0, v0);
            vIndex = this->writeVertex(vIndex, x1, y1, u1, v1);
            vIndex = this->writeVertex(vIndex, x0, y1, u0, v1);
        }
    }
    
    // write sprites
    const float canWidth = float(this->canvasWidth);
    const float canHeight = float(this->canvasHeight);
    for (int i = 0; i < this->numSprites; i++) {
        const sprite& sprite = this->sprites[i];
        float x0 = 0.0f;
        float y0 = 0.0f;
        float x1 = 0.0f;
        float y1 = 0.0f;
        float u0 = 0.0f;
        float u1 = 0.0f;
        float v0 = 0.0f;
        float v1 = 0.0f;
        if (Sheet::InvalidSprite != sprite.id) {
            const Sheet::sprite& s = Sheet::Sprite[sprite.id];
            x0 = float(sprite.x) / canWidth;
            y0 = float(sprite.y) / canHeight;
            x1 = x0 + float(sprite.w) / canWidth;
            y1 = y0 + float(sprite.h) / canHeight;
            u0 = float((s.X + s.W * sprite.frame) % Sheet::Width) / sheetWidth;
            v0 = float(s.Y + ((s.X + s.W * sprite.frame) / Sheet::Width) * s.H) / sheetHeight;
            u1 = u0 + float(s.W) / sheetWidth;
            v1 = v0 + float(s.H) / sheetHeight;
        }
        // write vertex data (2 triangles)
        o_assert_dbg((vIndex + 6) <= this->numVertices);
        vIndex = this->writeVertex(vIndex, x0, y0, u0, v0);
        vIndex = this->writeVertex(vIndex, x1, y0, u1, v0);
        vIndex = this->writeVertex(vIndex, x1, y1, u1, v1);
        vIndex = this->writeVertex(vIndex, x0, y0, u0, v0);
        vIndex = this->writeVertex(vIndex, x1, y1, u1, v1);
        vIndex = this->writeVertex(vIndex, x0, y1, u0, v1);
    }
    o_assert(vIndex == this->numVertices);
    outNumBytes = this->numVertices * sizeof(vertex);
    return this->vertexBuffer;
}

//------------------------------------------------------------------------------
void
canvas::CopyCharMap(int tileX, int tileY, int tileW, int tileH, const char* charMap) {
    const int x0 = this->ClampX(tileX);
    const int y0 = this->ClampY(tileY);
    const int x1 = this->ClampX(tileX + tileW);
    const int y1 = this->ClampY(tileY + tileH);
    for (int y = y0; y <= y1; y++) {
        for (int x = x0; x <= x1; x++) {
            const unsigned char c = *charMap++;
            o_assert_dbg(0 != c);
            Sheet::SpriteId sprite = Sheet::CharMap[c];
            if (sprite != Sheet::InvalidSprite) {
                this->tiles[y * this->numTilesX + x] = sprite;
            }
        }
    }
}

//------------------------------------------------------------------------------
void
canvas::SetTile(Sheet::SpriteId sprite, int tileX, int tileY) {
    const int x = this->ClampX(tileX);
    const int y = this->ClampY(tileY);
    this->tiles[y * this->numTilesX + x] = sprite;
}

//------------------------------------------------------------------------------
int
canvas::animate(Sheet::SpriteId spriteId, int animTick) const {
    o_assert(animTick >= 0);
    const auto& s = Sheet::Sprite[spriteId];
    o_assert(s.NumFrames > 0);
    int frame;
    if (s.AnimType == Sheet::Anim::None) {
        frame = 0;
    }
    else if (s.AnimType == Sheet::Anim::Loop) {
        frame = animTick % s.NumFrames;
    }
    else if (s.AnimType == Sheet::Anim::Clamp) {
        frame = animTick < s.NumFrames ? animTick : s.NumFrames - 1;
    }
    else {
        // ping-pong
        int frame2 = animTick % (s.NumFrames * 2);
        frame  = animTick % s.NumFrames;
        if (frame2 >= s.NumFrames) {
            frame = (s.NumFrames - 1) - frame;
        }
    }
    o_assert((frame >= 0) && (frame < s.NumFrames));
    return frame;
}

//------------------------------------------------------------------------------
void
canvas::SetSprite(int index, Sheet::SpriteId spriteId, int pixX, int pixY, int pixW, int pixH, int animTick) {
    o_assert_range(index, this->numSprites);
    o_assert(spriteId < Sheet::NumSprites);
    this->sprites[index].id = spriteId;
    this->sprites[index].x = pixX;
    this->sprites[index].y = pixY;
    this->sprites[index].w = pixW;
    this->sprites[index].h = pixH;
    this->sprites[index].frame = this->animate(spriteId, animTick);
}

//------------------------------------------------------------------------------
void
canvas::ClearSprite(int index) {
    o_assert_range(index, this->numSprites);
    this->sprites[index].id = Sheet::InvalidSprite;
}

//------------------------------------------------------------------------------
int
canvas::ClampX(int tileX) const {
    if (tileX < 0) return 0;
    else if (tileX >= this->numTilesX) return this->numTilesX - 1;
    else return tileX;
}

//------------------------------------------------------------------------------
int
canvas::ClampY(int tileY) const {
    if (tileY < 0) return 0;
    else if (tileY >= this->numTilesY) return this->numTilesY - 1;
    else return tileY;
}

//------------------------------------------------------------------------------
int
canvas::CanvasWidth() const {
    return this->canvasWidth;
}

//------------------------------------------------------------------------------
int
canvas::CanvasHeight() const {
    return this->canvasHeight;
}

} // namespace Paclone
