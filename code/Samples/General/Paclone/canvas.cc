//------------------------------------------------------------------------------
//  canvas.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "canvas.h"

using namespace Oryol;

namespace Paclone {
    
//------------------------------------------------------------------------------
canvas::canvas(int width, int height) {
    for (int y = 0; y < Sheet::Height; y++) {
        for (int x = 0; x < Sheet::Width; x++) {
            this->tiles[y][x] = Sheet::InvalidSprite;
        }
    }
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
            this->tiles[y][x] = sprite;
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
        this->tiles[y][x] = this->MergeWallPiece(spriteId, this->tiles[y][x]);
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
            spriteId = Sheet::Wall_1_010_0;
        }
        else if (y == y1) {
            // end piece
            spriteId = Sheet::Wall_0_010_1;
        }
        else if (y0 == y1) {
            // just a dot
            spriteId = Sheet::Wall_0_010_0;
        }
        else {
            // middle piece
            spriteId = Sheet::Wall_1_010_1;
        }
        this->tiles[y][x] = this->MergeWallPiece(spriteId, this->tiles[y][x]);
    }
}
    
//------------------------------------------------------------------------------
Sheet::SpriteId
canvas::MergeWallPiece(Sheet::SpriteId piece0, Sheet::SpriteId piece1) {
    // FIXME!
    return piece0;
}
    
//------------------------------------------------------------------------------
void
canvas::Set(int x, int y, Sheet::SpriteId sprite) {
    o_assert((x < Sheet::Width) && (y < Sheet::Height));
    this->tiles[y][x] = sprite;
}
    
//------------------------------------------------------------------------------
Sheet::SpriteId
canvas::Get(int x, int y) const {
    o_assert((x < Sheet::Width) && (y < Sheet::Height));
    return this->tiles[y][x];
}
    
//------------------------------------------------------------------------------
int
canvas::ClampX(int x) {
    if (x < 0) return 0;
    else if (x >= Sheet::Width) return Sheet::Width - 1;
    else return x;
}

//------------------------------------------------------------------------------
int
canvas::ClampY(int y) {
    if (y < 0) return 0;
    else if (y >= Sheet::Height) return Sheet::Height - 1;
    else return y;
}

} // namespace Paclone
