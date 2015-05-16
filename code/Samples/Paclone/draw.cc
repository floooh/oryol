//------------------------------------------------------------------------------
//  draw.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "draw.h"
#include "func.h"
#include "Dbg/Dbg.h"

using namespace Oryol;

namespace Paclone {

//------------------------------------------------------------------------------
void
draw::initMap(const state& state, canvas* canvas) {
    o_assert_dbg(canvas);
    canvas->CopyCharMap(0, 0, Width, Height, state.charMap);
}

//------------------------------------------------------------------------------
void
draw::drawActors(const state& state, canvas* canvas) {
    o_assert_dbg(canvas);
    for (const Actor& actor : state.actors) {
        Direction dir = (actor.type == Pacman ? actor.dir : actor.nextDir);
        Sheet::SpriteId spriteId = Sheet::InvalidSprite;
        int animSpeed = (actor.type == Pacman) ? 2 : 4;
        int animTick  = actor.moveTick / animSpeed;
        if ((actor.type == Pacman) && (actor.killedTicks > 0)) {
            spriteId = Sheet::PacmanDead;
            animTick = (PacmanDeathTicks - actor.killedTicks) / 5;
        }
        else if ((actor.state == Hollow) || (actor.state == EnterHouse)) {
            if (actor.killedTicks > 0) {
                o_assert_dbg((actor.killedCount > 0) && (actor.killedCount <= NumGhosts));
                spriteId = state.killedGhostMap[actor.killedCount - 1];
            }
            else {
                spriteId = state.hollowSpriteMap[dir];
            }
        }
        else if (actor.frightenedTick > 0) {
            spriteId = Sheet::FrightenedGhost;
            if ((actor.frightenedTick < (2 * 60)) && ((actor.frightenedTick / 15) % 2)) {
                spriteId = Sheet::FlashingGhost;
            }
        }
        else {
            spriteId = state.defaultSpriteMap[actor.type][dir];
        }
        if (Sheet::InvalidSprite != spriteId) {
            const Int2 pos = func::pixelDrawPos(actor.pixelPos);
            const Int2 size{2 * TileSize, 2 * TileSize};
            canvas->SetSprite(actor.spriteIndex, spriteId, pos.x, pos.y, size.x, size.y, animTick);
        }
    }
}
    
//------------------------------------------------------------------------------
void
draw::drawEnergizers(const state& state, canvas* canvas) {
    o_assert_dbg(canvas);
    for (const Energizer& cur : state.energizers) {
        if (cur.active) {
            const Int2 pos = func::tileToPixelPos(cur.tilePos);
            const Int2 size{2 * TileSize, 2 * TileSize};
            canvas->SetSprite(cur.spriteIndex, Sheet::Energizer, pos.x, pos.y, size.x, size.y, 0);
        }
        else {
            canvas->ClearSprite(cur.spriteIndex);
        }
    }
}

//------------------------------------------------------------------------------
void
draw::drawChrome(const state& state, canvas* canvas) {
    o_assert_dbg(canvas);
    
    // draw the playfield chrome (highscore, lives, ...)
    Dbg::CursorPos(9, 0);
    Dbg::TextColor(glm::vec4(1.0f));
    Dbg::Print("HIGH SCORE");
    
    // current score and hiscore
    Dbg::CursorPos(0, 1);
    Dbg::PrintF("% 6d0", state.score);
    Dbg::CursorPos(10, 1);
    Dbg::PrintF("% 6d0", state.hiscore);
    
    // READY! string
    if (0 == state.gameTick) {
        Dbg::CursorPos(11, 20);
        Dbg::TextColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
        Dbg::Print("READY!");
    }
    
    // draw lives
    const int baseIndex = NumActorTypes + NumEnergizers;
    int pixX = 0;
    const int pixY = 34 * TileSize;
    const int size = 2 * TileSize;
    int i;
    for (i = 0; i < state.lives; i++) {
        canvas->SetSprite(baseIndex + i, Sheet::PacmanLeft, pixX, pixY, size, size, 1);
        pixX += size;
    }
    for (; i < NumLives; i++) {
        canvas->ClearSprite(baseIndex + i);
    }
}
    
} // namespace Paclone