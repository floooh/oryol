//------------------------------------------------------------------------------
//  game.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "game.h"

using namespace Oryol;

namespace Paclone {
    
//------------------------------------------------------------------------------
game::game() {
    // empty
}

//------------------------------------------------------------------------------
void
game::Init(canvas* canvas) {
    o_assert(canvas);
    
    // FIXME: setup playfield
    canvas->FillRect(0, 0, 23, 16, Paclone::Sheet::Pill);
    canvas->HoriWall(0, 0, 22);
    canvas->HoriWall(0, 16, 22);
    canvas->VertWall(0, 0, 16);
    canvas->VertWall(22, 0, 16);
    canvas->RectWall(2, 2, 2, 1);
    canvas->RectWall(6, 2, 3, 1);
    canvas->VertWall(11, 0, 3);
    canvas->RectWall(13, 2, 3, 1);
    canvas->RectWall(18, 2, 2, 1);
    canvas->Set(10, 5, Paclone::Sheet::Blinky);
    canvas->Set(12, 5, Paclone::Sheet::Pinky);
    canvas->Set(10, 7, Paclone::Sheet::Clyde);
    canvas->Set(12, 7, Paclone::Sheet::Inky);
}

//------------------------------------------------------------------------------
void
game::Cleanup() {
    // FIXME!
}

//------------------------------------------------------------------------------
void
game::Update(int32 tick, canvas* canvas) {
    // FIXME!
}

} // namespace Paclone