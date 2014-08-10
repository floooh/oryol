//------------------------------------------------------------------------------
//  game.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "game.h"
#include <cstring>

using namespace Oryol;

namespace Paclone {

const char* charMap =
   /*0123456789012345678901234567*/
    "                            " // 0
    "                            " // 1
    "                            " // 2
    "############################" // 3
    "#............##............#" // 4
    "#.####.#####.##.#####.####.#" // 5
    "#.####.#####.##.#####.####.#" // 6
    "#.####.#####.##.#####.####.#" // 7
    "#..........................#" // 8
    "#.####.##.########.##.####.#" // 9
    "#.####.##.########.##.####.#" // 10
    "#......##....##....##......#" // 11
    "######.##### ## #####.######" // 12
    "######.##### ## #####.######" // 13
    "######.##          ##.######" // 14
    "######.## ######## ##.######" // 15
    "######.## ######## ##.######" // 16
    "      .   ########   .      " // 17
    "######.## ######## ##.######" // 18
    "######.## ######## ##.######" // 19
    "######.##          ##.######" // 20
    "######.## ######## ##.######" // 21
    "######.## ######## ##.######" // 22
    "#............##............#" // 23
    "#.####.#####.##.#####.####.#" // 24
    "#.####.#####.##.#####.####.#" // 25
    "#...##.......  .......##...#" // 26
    "###.##.##.########.##.##.###" // 27
    "###.##.##.########.##.##.###" // 28
    "#......##....##....##......#" // 29
    "#.##########.##.##########.#" // 30
    "#.##########.##.##########.#" // 31
    "#..........................#" // 32
    "############################" // 33
    "                            " // 34
    "                            ";// 35
    
//------------------------------------------------------------------------------
game::game() {
    o_assert(std::strlen(charMap) == Width * Height);
}

//------------------------------------------------------------------------------
void
game::Init(canvas* canvas) {
    o_assert(canvas);
    canvas->CopyCharMap(0, 0, Width, Height, charMap);
    this->setupTiles();
    this->setupActors();
}

//------------------------------------------------------------------------------
void
game::Cleanup() {
    // FIXME!
}

//------------------------------------------------------------------------------
void
game::Update(int tick, canvas* canvas, Direction input) {
    o_assert_dbg(canvas);
    this->updateActors(input);
    this->checkOverlaps(canvas);
    this->drawActors(canvas);
}

//------------------------------------------------------------------------------
void
game::setTile(int x, int y, TileType type) {
    if (x < 0) x = 0;
    else if (x >= Width) x = Width - 1;
    if (y < 0) y = 0;
    else if (y >= Height) y = Height - 1;
    this->tileMap[y][x] = type;
}


//------------------------------------------------------------------------------
game::TileType
game::tile(int x, int y) const {
    if (x < 0) x = 0;
    else if (x >= Width) x = Width - 1;
    if (y < 0) y = 0;
    else if (y >= Height) y = Height - 1;
    return this->tileMap[y][x];
}

//------------------------------------------------------------------------------
void
game::setupTiles() {
    for (int y = 0; y < Height; y++) {
        for (int x = 0; x < Width; x++) {
            char c = charMap[y * Width + x];
            TileType tileType = Empty;
            switch (c) {
                case '.': tileType = Pill; break;
                case ' ': tileType = Empty; break;
                default:  tileType = Wall; break;
            }
            this->setTile(x, y, tileType);
        }
    }
}

//------------------------------------------------------------------------------
void
game::setupActors() {
    this->setupActor(Pacman, 13, 26, NoDirection);
    this->setupActor(Ghost0, 13, 14, Left);
    this->setupActor(Ghost1, 11, 17, Up);
    this->setupActor(Ghost2, 13, 17, Down);
    this->setupActor(Ghost3, 15, 17, Up);
}

//------------------------------------------------------------------------------
void
game::setupActor(ActorType type, int x, int y, Direction dir) {
    o_assert_range(type, NumActorTypes);
    o_assert((x >= 0) && (x < Width));
    o_assert((y >= 0) && (y < Height));
    Actor& actor = this->actors[type];
    switch (type) {
        case Pacman: actor.spriteId = Sheet::TestPacman; break;
        case Ghost0: actor.spriteId = Sheet::TestGhost0; break;
        case Ghost1: actor.spriteId = Sheet::TestGhost1; break;
        case Ghost2: actor.spriteId = Sheet::TestGhost2; break;
        case Ghost3: actor.spriteId = Sheet::TestGhost3; break;
        default: actor.spriteId = Sheet::InvalidSprite; break;
    }
    actor.dir = dir;
    actor.x = x;
    actor.y = y;
    actor.subX = 4;
    actor.subY = 0;
}

//------------------------------------------------------------------------------
void
game::drawActors(canvas* canvas) {
    this->drawActor(Pacman, canvas);
    this->drawActor(Ghost0, canvas);
    this->drawActor(Ghost1, canvas);
    this->drawActor(Ghost2, canvas);
    this->drawActor(Ghost3, canvas);    
}

//------------------------------------------------------------------------------
void
game::drawActor(ActorType type, canvas* canvas) {
    o_assert_range_dbg(type, NumActorTypes);
    o_assert_dbg(canvas);
    
    const Actor& actor = this->actors[type];
    if (actor.spriteId != Sheet::InvalidSprite) {
        const int pixX = (actor.x * 8 + actor.subX) - 4;
        const int pixY = (actor.y * 8 + actor.subY) - 4;
        const int pixW = 2 * 8;
        const int pixH = 2 * 8;
        canvas->SetSprite(type, actor.spriteId, pixX, pixY, pixW, pixH);
    }
}

//------------------------------------------------------------------------------
void
game::updateActors(Direction input) {
    this->updatePacman(input);
}

//------------------------------------------------------------------------------
void
game::updatePacman(Direction input) {
    // pacman can't stop
    Actor& pacman = this->actors[Pacman];
    this->computeMove(pacman, input);
    this->move(pacman);
}

//------------------------------------------------------------------------------
void
game::move(Actor& actor) {
    actor.subX += actor.dx;
    actor.subY += actor.dy;
    if (actor.subX < 0) {
        actor.x--;
        actor.subX = 7;
    }
    else if (actor.subX > 7) {
        actor.x++;
        actor.subX = 0;
    }
    if (actor.subY < 0) {
        actor.y--;
        actor.subY = 7;
    }
    else if (actor.subY > 7) {
        actor.y++;
        actor.subY = 0;
    }
    if (actor.x < 0) {
        // wraparound!
        actor.x = Width - 1;
    }
    else if (actor.x >= Width) {
        // wraparound!
        actor.x = 0;
    }
    if (actor.y < 0) {
        actor.y = 0;
    }
    else if (actor.y >= Height) {
        actor.y = Height - 1;
    }
    
}

//------------------------------------------------------------------------------
/**
 Test if movement in a given direction is possible.
 FIXME: take cornering into account.
*/
bool
game::canMove(const Actor& actor, Direction dir) {
    if (Up == dir) {
        if ((0 != actor.subX) || ((0 == actor.subY) && (Wall == this->tile(actor.x, actor.y - 1)))) {
            return false;
        }
    }
    else if (Down == dir) {
        if ((0 != actor.subX) || ((0 == actor.subY) && (Wall == this->tile(actor.x, actor.y + 1)))) {
            return false;
        }
    }
    else if (Left == dir) {
        if ((0 != actor.subY) || ((0 == actor.subX) && (Wall == this->tile(actor.x - 1, actor.y)))) {
            return false;
        }
    }
    else if (Right == dir) {
        if ((0 != actor.subY) || ((0 == actor.subX) && (Wall == this->tile(actor.x + 1, actor.y)))) {
            return false;
        }
    }
    return true;
}

//------------------------------------------------------------------------------
/**
 Movement rules for Pacman and Ghosts are different: see 'Cornering':
 http://home.comcast.net/~jpittman2/pacman/pacmandossier.html#CH2_Cornering
*/
void
game::computeMove(Actor& actor, Direction dir) {

    // dir is the direction the actor *wants* to move,
    // which might not be possible
    if ((NoDirection != dir) && this->canMove(actor, dir)) {
        // can move in intended direction, update actor direction
        // and movement vector
        actor.dir = dir;
        switch (actor.dir) {
            case Left:  actor.dx = -1; actor.dy = 0; break;
            case Right: actor.dx = +1; actor.dy = 0; break;
            case Up:    actor.dx = 0; actor.dy = -1; break;
            case Down:  actor.dx = 0; actor.dy = +1; break;
            default:    actor.dx = 0; actor.dy = 0; break;
        }
    }
    else {
        // movement into intended direction is not possible, check
        // whether movement in current direction is possible
        if (!this->canMove(actor, actor.dir)) {
            // nope, emergency stop
            actor.dx = 0;
            actor.dy = 0;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Checks for collission between pacman and ghosts, and between pacman and
    pills.
*/
void
game::checkOverlaps(canvas* canvas) {
    switch (this->tile(this->actors[Pacman].x, this->actors[Pacman].y)) {
        case Pill:
            // pacman eats a pill
            this->eatPill(canvas);
            break;
            
        case Energizer:
            // pacman eats an energizer
            this->eatEnergizer(canvas);
            break;
            
        default:
            break;
    }
    
    // FIXME: check for ghost overlaps
}

//------------------------------------------------------------------------------
void
game::eatPill(canvas* canvas) {
    int x = this->actors[Pacman].x;
    int y = this->actors[Pacman].y;
    this->setTile(x, y, Empty);
    canvas->SetTile(Sheet::Space, x, y);
}

//------------------------------------------------------------------------------
void
game::eatEnergizer(canvas* canvas) {
    // FIXME!
}

} // namespace Paclone
