#pragma once
//------------------------------------------------------------------------------
/**
    XY-Canvas for sprite tiles.
*/
#include "Core/Assert.h"
#include "sprites.h"

namespace Paclone {
    
class canvas {
public:
    // constructor
    canvas(int width, int height);
    // fill a rectangle with a sprite id
    void FillRect(int x, int y, int w, int h, Sheet::SpriteId sprite);
    // draw a horizontal wall
    void HoriWall(int x, int y, int w);
    // draw a vertical wall
    void VertWall(int x, int y, int h);
    // merge 2 wall pieces
    Sheet::SpriteId MergeWallPiece(Sheet::SpriteId piece0, Sheet::SpriteId piece1);
    // set sprite id at position
    void Set(int x, int y, Sheet::SpriteId sprite);
    // get sprite id at position
    Sheet::SpriteId Get(int x, int y) const;
    // clamp x-coordinate to valid area
    int ClampX(int x);
    // clamp y-coordinate to valid area
    int ClampY(int y);

private:
    Sheet::SpriteId tiles[Sheet::Height][Sheet::Width];
};

} // namespace Paclone

