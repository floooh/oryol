//------------------------------------------------------------------------------
//  canvas.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "canvas.h"
#include "shaders.h"
#include "Render/RenderFacade.h"
#include "IO/Stream/MemoryStream.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::IO;
using namespace Oryol::Render;

namespace Paclone {

const Sheet::SpriteId canvas::wallMergeMap[NumWallPieces] = {
    Sheet::InvalidSprite,   // Sheet::Wall_0_000_0
    Sheet::InvalidSprite,   // Sheet::Wall_0_000_1
    Sheet::InvalidSprite,   // Sheet::Wall_0_001_0
    Sheet::InvalidSprite,   // Sheet::Wall_0_001_1
    Sheet::Wall_0_010_0,
    Sheet::Wall_0_010_1,
    Sheet::Wall_0_011_0,
    Sheet::Wall_0_011_1,
    Sheet::InvalidSprite,   // Sheet::Wall_0_100_0
    Sheet::InvalidSprite,   // Sheet::Wall_0_100_1
    Sheet::InvalidSprite,   // Sheet::Wall_0_101_0
    Sheet::InvalidSprite,   // Sheet::Wall_0_101_1
    Sheet::Wall_0_110_0,
    Sheet::Wall_0_110_1,
    Sheet::Wall_0_111_0,
    Sheet::Wall_0_111_1,
    Sheet::InvalidSprite,   // Sheet::Wall_1_000_0
    Sheet::InvalidSprite,   // Sheet::Wall_1_000_1
    Sheet::InvalidSprite,   // Sheet::Wall_1_001_0
    Sheet::InvalidSprite,   // Sheet::Wall_1_001_1
    Sheet::Wall_1_010_0,
    Sheet::Wall_1_010_1,
    Sheet::Wall_1_011_0,
    Sheet::Wall_1_011_1,
    Sheet::InvalidSprite,   // Sheet::Wall_1_100_0
    Sheet::InvalidSprite,   // Sheet::Wall_1_100_1
    Sheet::InvalidSprite,   // Sheet::Wall_1_101_0
    Sheet::InvalidSprite,   // Sheet::Wall_1_101_1
    Sheet::Wall_1_110_0,
    Sheet::Wall_1_110_1,
    Sheet::Wall_1_111_0,
    Sheet::Wall_1_111_1,
};

//------------------------------------------------------------------------------
canvas::canvas() :
isValid(false),
width(0),
height(0),
numVertices(0) {
    // empty
}

//------------------------------------------------------------------------------
void
canvas::Setup(int w, int h) {
    o_assert(!this->isValid);
    o_assert((w > 0) && (w <= MaxWidth) && (h > 0) && (h <= MaxHeight));
    
    this->isValid = true;
    this->width = w;
    this->height = h;
    this->numVertices = this->width * this->height * 6;
    
    // setup draw state with dynamic mesh
    RenderFacade* render = RenderFacade::Instance();
    MeshSetup meshSetup = MeshSetup::CreateEmpty("p_canvas", this->numVertices, Usage::Dynamic);
    meshSetup.Layout.Add(VertexAttr::Position, VertexFormat::Float2);
    meshSetup.Layout.Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    meshSetup.AddPrimitiveGroup(PrimitiveGroup(PrimitiveType::Triangles, 0, this->numVertices));
    this->mesh = render->CreateResource(meshSetup);
    this->prog = render->CreateResource(Shaders::Main::CreateSetup());
    DrawStateSetup dsSetup("p_ds", this->mesh, this->prog, 0);
    dsSetup.BlendState.BlendEnabled = true;
    dsSetup.BlendState.SrcFactorRGB = BlendFactor::SrcAlpha;
    dsSetup.BlendState.DstFactorRGB = BlendFactor::OneMinusSrcAlpha;
    this->drawState = render->CreateResource(dsSetup);
    
    // setup sprite texture
    auto pixelData = MemoryStream::Create();
    pixelData->Open(OpenMode::WriteOnly);
    void* ptr = pixelData->MapWrite(Sheet::NumBytes);
    Memory::Copy(Sheet::Pixels, ptr, Sheet::NumBytes);
    pixelData->UnmapWrite();
    pixelData->Close();
    TextureSetup texSetup = TextureSetup::FromPixelData("p_pixels", Sheet::Width, Sheet::Height, false, PixelFormat::RGBA8);
    texSetup.MinFilter = TextureFilterMode::Nearest;
    texSetup.MagFilter = TextureFilterMode::Nearest;
    texSetup.WrapU = TextureWrapMode::ClampToEdge;
    texSetup.WrapV = TextureWrapMode::ClampToEdge;
    this->texture = render->CreateResource(texSetup, pixelData);
    
    // initialize the tile map
    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            this->tiles[y * this->width + x] = Sheet::InvalidSprite;
        }
    }
    
    // clear the vertex buffer
    Core::Memory::Clear(this->vertexBuffer, sizeof(this->vertexBuffer));
}

//------------------------------------------------------------------------------
void
canvas::Discard() {
    o_assert(this->isValid);
    this->isValid = false;
    RenderFacade* render = RenderFacade::Instance();
    render->ReleaseResource(this->drawState);
    render->ReleaseResource(this->prog);
    render->ReleaseResource(this->mesh);
    render->ReleaseResource(this->texture);
    this->drawState.Invalidate();
    this->prog.Invalidate();
    this->mesh.Invalidate();
}

//------------------------------------------------------------------------------
bool
canvas::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
int
canvas::Width() const {
    return this->width;
}

//------------------------------------------------------------------------------
int
canvas::Height() const {
    return this->height;
}
    
//------------------------------------------------------------------------------
void
canvas::Update() {
    o_assert(this->isValid);
    int32 numBytes = 0;
    const void* data = this->updateVertices(numBytes);
    RenderFacade::Instance()->UpdateVertices(this->mesh, numBytes, data);
}

//------------------------------------------------------------------------------
void
canvas::Render() {
    o_assert(this->isValid);
    RenderFacade* render = RenderFacade::Instance();
    render->ApplyDrawState(this->drawState);
    render->ApplyVariable(Shaders::Main::Texture, this->texture);
    render->Draw(0);
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
canvas::updateVertices(int32& outNumBytes) {

    // write the tile map
    const float dx = 1.0f / this->width;
    const float dy = 1.0f / this->height;
    const float sheetWidth = float(Sheet::Width);
    const float sheetHeight = float(Sheet::Height);
    for (int y = 0; y < this->height; y++) {
        const float y0 = dy * y;
        const float y1 = y0 + dy;
        for (int x = 0; x < this->width; x++) {
            
            // positions
            const float x0 = dx * x;
            const float x1 = x0 + dx;
            
            // uvs
            Sheet::SpriteId id = this->tiles[y * this->width + x];
            const auto& sprite = Sheet::Sprite[id];
            const float u0 = float(sprite.X) / sheetWidth;
            const float v0 = float(sprite.Y) / sheetHeight;
            const float u1 = u0 + float(sprite.W) / sheetWidth;
            const float v1 = v0 + float(sprite.H) / sheetHeight;
            
            // write vertex data (2 triangles)
            int vi = (y * this->width + x) * 6;
            o_assert_dbg((vi + 6) <= this->numVertices);
            vi = this->writeVertex(vi, x0, y0, u0, v0);
            vi = this->writeVertex(vi, x1, y0, u1, v0);
            vi = this->writeVertex(vi, x1, y1, u1, v1);
            vi = this->writeVertex(vi, x0, y0, u0, v0);
            vi = this->writeVertex(vi, x1, y1, u1, v1);
            vi = this->writeVertex(vi, x0, y1, u0, v1);
        }
    }
    
    outNumBytes = this->width * this->height * 6 * sizeof(vertex);
    return this->vertexBuffer;
}

//------------------------------------------------------------------------------
void
canvas::FillRect(int x, int y, int w, int h, Sheet::SpriteId sprite) {
    int x0 = this->ClampX(x);
    int y0 = this->ClampY(y);
    int x1 = this->ClampX(x + w);
    int y1 = this->ClampY(y + h);
    for (y = y0; y <= y1; y++) {
        for (x = x0; x <= x1; x++) {
            this->tiles[y * this->width + x] = sprite;
        }
    }
}
    
//------------------------------------------------------------------------------
void
canvas::HoriWall(int x, int y, int w) {
    int x0 = this->ClampX(x);
    int x1 = this->ClampX(x + w);
    y = this->ClampY(y);
    for (x = x0; x <= x1; x++) {
        Sheet::SpriteId spriteId;
        if (x == x0) {
            // start piece
            spriteId = Sheet::Wall_0_011_0;
        }
        else if (x == x1) {
            // end piece
            spriteId = Sheet::Wall_0_110_0;
        }
        else if (x0 == x1) {
            // just a dot
            spriteId = Sheet::Wall_0_010_0;
        }
        else {
            // middle piece
            spriteId = Sheet::Wall_0_111_0;
        }
        int tileIndex = y * this->width + x;
        this->tiles[tileIndex] = this->MergeWallPiece(spriteId, this->tiles[tileIndex]);
    }
}
    
//------------------------------------------------------------------------------
void
canvas::VertWall(int x, int y, int h) {
    int y0 = this->ClampY(y);
    int y1 = this->ClampY(y + h);
    x = this->ClampX(x);
    for (y = y0; y <= y1; y++) {
        Sheet::SpriteId spriteId;
        if (y == y0) {
            // start piece
            spriteId = Sheet::Wall_0_010_1;
        }
        else if (y == y1) {
            // end piece
            spriteId = Sheet::Wall_1_010_0;
        }
        else if (y0 == y1) {
            // just a dot
            spriteId = Sheet::Wall_0_010_0;
        }
        else {
            // middle piece
            spriteId = Sheet::Wall_1_010_1;
        }
        int tileIndex = y * this->width + x;
        this->tiles[tileIndex] = this->MergeWallPiece(spriteId, this->tiles[tileIndex]);
    }
}

//------------------------------------------------------------------------------
void
canvas::RectWall(int x, int y, int w, int h) {
    this->HoriWall(x, y, w);
    this->VertWall(x, y, h);
    this->HoriWall(x, y + h, w);
    this->VertWall(x + w, y, h);
}
    
//------------------------------------------------------------------------------
Sheet::SpriteId
canvas::MergeWallPiece(Sheet::SpriteId piece0, Sheet::SpriteId piece1) const {
    o_assert_range(piece0, Sheet::NumSprites);
    o_assert_range(piece1, Sheet::NumSprites);
    const uint32 mask0 = Sheet::Sprite[piece0].Mask;
    const uint32 mask1 = Sheet::Sprite[piece1].Mask;
    o_assert(mask0 < NumWallPieces);
    o_assert(mask1 < NumWallPieces);
    Sheet::SpriteId mergedPiece = this->wallMergeMap[mask0 | mask1];
    return mergedPiece;
}
    
//------------------------------------------------------------------------------
void
canvas::Set(int x, int y, Sheet::SpriteId sprite) {
    x = this->ClampX(x);
    y = this->ClampY(y);
    this->tiles[y * this->width + x] = sprite;
}
    
//------------------------------------------------------------------------------
Sheet::SpriteId
canvas::Get(int x, int y) const {
    o_assert((x < this->width) && (y < this->height));
    return this->tiles[y * this->width + x];
}
    
//------------------------------------------------------------------------------
int
canvas::ClampX(int x) const {
    if (x < 0) return 0;
    else if (x >= this->width) return this->width - 1;
    else return x;
}

//------------------------------------------------------------------------------
int
canvas::ClampY(int y) const {
    if (y < 0) return 0;
    else if (y >= this->height) return this->height - 1;
    else return y;
}

} // namespace Paclone
