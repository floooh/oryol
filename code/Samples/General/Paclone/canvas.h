#pragma once
//------------------------------------------------------------------------------
/**
    XY-Canvas for sprite tiles.
*/
#include "Core/Assert.h"
#include "Render/Setup/MeshSetup.h"
#include "sprites.h"

namespace Paclone {
    
class canvas {
public:
    // constructor
    canvas();
    
    /// setup the canvas
    void Setup(int width, int height);
    /// discard the cancas
    void Discard();
    /// return true if canvas has been setup
    bool IsValid() const;
    /// update the canvas
    void Update();
    /// render the canvas
    void Render();
    
    /// get width of canvas (in number of sprites)
    int Width() const;
    /// get height of canvas (in number of sprites)
    int Height() const;

    // fill a rectangle with a sprite id
    void FillRect(int x, int y, int w, int h, Sheet::SpriteId sprite);
    // draw a horizontal wall
    void HoriWall(int x, int y, int w);
    // draw a vertical wall
    void VertWall(int x, int y, int h);
    /// draw a wall rectangle
    void RectWall(int x, int y, int w, int h);
    // merge 2 wall pieces
    Sheet::SpriteId MergeWallPiece(Sheet::SpriteId piece0, Sheet::SpriteId piece1) const;
    // set sprite id at position
    void Set(int x, int y, Sheet::SpriteId sprite);
    // get sprite id at position
    Sheet::SpriteId Get(int x, int y) const;
    // clamp x-coordinate to valid area
    int ClampX(int x) const;
    // clamp y-coordinate to valid area
    int ClampY(int y) const;

private:
    /// write a vertex
    int writeVertex(int index, float x, float y, float u, float v);
    /// update vertices, returns pointer to and size of vertex data
    const void* updateVertices(Oryol::int32& outNumBytes);
    
    bool isValid;
    int width;
    int height;
    int numVertices;
    Oryol::Resource::Id mesh;
    Oryol::Resource::Id prog;
    Oryol::Resource::Id drawState;
    Oryol::Resource::Id texture;

    static const int MaxWidth = 32;
    static const int MaxHeight = 32;
    Sheet::SpriteId tiles[MaxWidth * MaxHeight];
    
    static const int NumWallPieces = 32;
    static const Sheet::SpriteId wallMergeMap[NumWallPieces];

    struct vertex {
        float x, y, u, v;
    };
    static const int MaxNumVertices = MaxWidth * MaxHeight * 6;
    vertex vertexBuffer[MaxNumVertices];
};

} // namespace Paclone

