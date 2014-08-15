//------------------------------------------------------------------------------
//  game.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "game.h"
#include <cstring>
#include <cstdlib>

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
    "######.## #      # ##.######" // 16
    "      .   #      #   .      " // 17
    "######.## #      # ##.######" // 18
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
    this->ghostModeTick++;
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
    this->setupActor(Blinky, 13, 14, NoDirection);
    this->setupActor(Pinky, 13, 14, NoDirection);
    this->setupActor(Inky, 13, 14, NoDirection);
    this->setupActor(Clyde, 13, 14, NoDirection);
}

//------------------------------------------------------------------------------
void
game::setupActor(ActorType type, int tileX, int tileY, Direction dir) {
    o_assert_range(type, NumActorTypes);
    o_assert((tileX >= 0) && (tileX < Width));
    o_assert((tileY >= 0) && (tileY < Height));
    Actor& actor = this->actors[type];
    actor.actorType = type;
    actor.dir = dir;
    actor.nextDir = dir;
    actor.pixelX = tileX * TileSize + TileMidX + TileSize/2;
    actor.pixelY = tileY * TileSize + TileMidY;
    actor.ghostMode = Scatter;
    this->commitPosition(actor);
}

//------------------------------------------------------------------------------
void
game::drawActors(canvas* canvas) {
    this->drawActor(Pacman, canvas);
    this->drawActor(Blinky, canvas);
    this->drawActor(Pinky, canvas);
    this->drawActor(Inky, canvas);
    this->drawActor(Clyde, canvas);
}

//------------------------------------------------------------------------------
void
game::drawActor(ActorType type, canvas* canvas) {
    o_assert_range_dbg(type, NumActorTypes);
    o_assert_dbg(canvas);
    
    const Actor& actor = this->actors[type];
    Sheet::SpriteId spriteId = Sheet::InvalidSprite;
    if (actor.actorType == Pacman) {
        if (actor.dir == Left) spriteId = Sheet::PacmanLeft;
        else if (actor.dir == Right) spriteId = Sheet::PacmanRight;
        else if (actor.dir == Up) spriteId = Sheet::PacmanUp;
        else spriteId = Sheet::PacmanDown;
    }
    else if (actor.actorType == Blinky) {
        spriteId = Sheet::Blinky;
    }
    else if (actor.actorType == Pinky) {
        spriteId = Sheet::Pinky;
    }
    else if (actor.actorType == Inky) {
        spriteId = Sheet::Inky;
    }
    else if (actor.actorType == Clyde) {
        spriteId = Sheet::Clyde;
    }
    if (Sheet::InvalidSprite != spriteId) {
        const int pixX = actor.pixelX - TileMidX - TileSize/2;
        const int pixY = actor.pixelY - TileMidY - TileSize/2;
        const int pixW = 2 * 8;
        const int pixH = 2 * 8;
        canvas->SetSprite(type, spriteId, pixX, pixY, pixW, pixH, actor.moveTick);
    }
}

//------------------------------------------------------------------------------
void
game::updateActors(Direction input) {
    this->updatePacman(input);
    this->selectGhostMode();
    this->updateGhost(this->actors[Blinky]);
    this->updateGhost(this->actors[Pinky]);
    this->updateGhost(this->actors[Inky]);
    this->updateGhost(this->actors[Clyde]);
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
game::updateGhost(Actor& ghost) {
    if (Scatter == ghost.ghostMode) {
        this->computeScatterTarget(ghost);
    }
    else if (Chase == ghost.ghostMode) {
        this->computeChaseTarget(ghost);
    }
    this->chooseDirection(ghost);
    this->computeMove(ghost, ghost.dir);
    this->move(ghost);
}

//------------------------------------------------------------------------------
game::Direction
game::reverseDir(Direction dir) {
    switch (dir) {
        case Left: return Right;
        case Right: return Left;
        case Up: return Down;
        case Down: return Up;
        default: return NoDirection;
    }
}

//------------------------------------------------------------------------------
void
game::selectGhostMode() {

    const int scatterFrames = 7 * 60;
    const int chaseFrames = 20 * 60;
    int tick = this->ghostModeTick % (scatterFrames + chaseFrames);
    GhostMode mode = Scatter;
    if (tick >= scatterFrames) {
        mode = Chase;
    }
    for (int i = Blinky; i <= Clyde; i++) {
        if (this->actors[i].ghostMode != mode) {
            // invert current direction
            this->actors[i].nextDir = this->reverseDir(this->actors[i].dir);
        }
        this->actors[i].ghostMode = mode;
    }
}

//------------------------------------------------------------------------------
void
game::computeScatterTarget(Actor& ghost) {
    switch (ghost.actorType) {
        case Blinky:
            ghost.targetX = 25;
            ghost.targetY = 0;
            break;
            
        case Pinky:
            ghost.targetX = 2;
            ghost.targetY = 0;
            break;
            
        case Inky:
            ghost.targetX = 27;
            ghost.targetY = 34;
            break;
        
        case Clyde:
            ghost.targetX = 0;
            ghost.targetY = 34;
            break;
            
        default:
            ghost.targetX = 0;
            ghost.targetY = 0;
            break;
    }
}

//------------------------------------------------------------------------------
void
game::computeChaseTarget(Actor& ghost) {
    const Actor& pacman = this->actors[Pacman];
    if (Blinky == ghost.actorType) {
        // directly target pacman
        ghost.targetX = pacman.tileX;
        ghost.targetY = pacman.tileY;
    }
    else if (Pinky == ghost.actorType) {
        // target 4 tiles ahead of pacman, with exception of 'up'
        ghost.targetX = pacman.tileX;
        ghost.targetX = pacman.tileX;
        switch (pacman.dir) {
            case Left:
                ghost.targetX = pacman.tileX - 4;
                break;
            case Right:
                ghost.targetX = pacman.tileX + 4;
                break;
            case Up:
                // overflow bug
                ghost.targetX = pacman.tileX - 4;
                ghost.targetY = pacman.tileY - 4;
                break;
            case Down:
                ghost.targetY = pacman.tileY + 4;
                break;
            default:
                break;
        }
    }
    else if (Inky == ghost.actorType) {
        int mx = pacman.tileX;
        int my = pacman.tileY;
        switch (pacman.dir) {
            case Left:
                mx = pacman.tileX - 2;
                break;
            case Right:
                mx = pacman.tileX + 2;
                break;
            case Up:
                // overflow bug
                mx = pacman.tileX - 2;
                my = pacman.tileY - 2;
                break;
            case Down:
                my = pacman.tileY + 2;
                break;
            default:
                break;
        }
        const Actor& blinky = this->actors[Blinky];
        int dx = mx - blinky.tileX;
        int dy = my - blinky.tileY;
        ghost.targetX = blinky.tileX + 2 * dx;
        ghost.targetY = blinky.tileY + 2 * dy;
    }
    else if (Clyde == ghost.actorType) {
        int dx = ghost.tileX - pacman.tileX;
        int dy = ghost.tileY - pacman.tileY;
        if ((dx*dx + dy*dy) > 64) {
            ghost.targetX = pacman.tileX;
            ghost.targetY = pacman.tileY;
        }
        else {
            ghost.targetX = 0;
            ghost.targetY = 34;
        }
    }
}

//------------------------------------------------------------------------------
int
game::targetDist(const Actor& ghost, int x, int y) {
    int dx = std::abs(x - ghost.targetX );
    int dy = std::abs(y - ghost.targetY);
    return dx + dy;
}

//------------------------------------------------------------------------------
void
game::chooseDirection(Actor& ghost) {
    if (((ghost.distToMidX == 0) && (ghost.distToMidY == 0)) || (NoDirection == ghost.dir)) {
        if (NoDirection != ghost.nextDir) {
            ghost.dir = ghost.nextDir;
        }
        
        // look ahead one tile to find next direction
        int16 x = ghost.tileX;
        int16 y = ghost.tileY;
        if (Wall != this->tile(x, y)) {
            switch (ghost.dir) {
                case Left:  x--; break;
                case Right: x++; break;
                case Up:    y--; break;
                case Down:  y++; break;
                default: break;
            }
        }
        
        // now test distance of each possible target tile
        // note: directions have a preference in case of a tie:
        // UP -> LEFT -> DOWN -> RIGHT
        Direction bestDirection = NoDirection;
        int minDist = 1000;
        int dist;
        if ((Down != ghost.dir) && (Wall != this->tile(x, y - 1))) {
            if ((dist = this->targetDist(ghost, x, y - 1)) < minDist) {
                bestDirection = Up;
                minDist = dist;
            }
        }
        if ((Right != ghost.dir) && (Wall != this->tile(x - 1, y))) {
            if ((dist = this->targetDist(ghost, x - 1, y)) < minDist) {
                bestDirection = Left;
                minDist = dist;
            }
        }
        if ((Up != ghost.dir) && (Wall != this->tile(x, y + 1))) {
            if ((dist = this->targetDist(ghost, x, y + 1)) < minDist) {
                bestDirection = Down;
                minDist = dist;
            }
        }
        if ((Left != ghost.dir) && (Wall != this->tile(x + 1, y))) {
            if ((dist = this->targetDist(ghost, x + 1, y)) < minDist) {
                bestDirection = Right;
                minDist = dist;
            }
        }
        ghost.nextDir = bestDirection;
    }
}

//------------------------------------------------------------------------------
void
game::move(Actor& actor) {
    actor.pixelX += actor.dirX;
    actor.pixelY += actor.dirY;
    if (actor.dir != NoDirection) {
        actor.moveTick++;
    }

    // cornering
    if (actor.dirX != 0) {
        if (actor.distToMidY < 0)       actor.pixelY--;
        else if (actor.distToMidY > 0)  actor.pixelY++;
    }
    else if (actor.dirY != 0) {
        if (actor.distToMidX < 0)       actor.pixelX--;
        else if (actor.distToMidX > 0)  actor.pixelX++;
    }
    
    // wrap around X for teleport (not 100% right I think)
    if (actor.pixelX < 0) actor.pixelX = Width * TileSize - 1;
    else if (actor.pixelX > Width * TileSize) actor.pixelX = 0;
    
    this->commitPosition(actor);
}

//------------------------------------------------------------------------------
/**
 Update tile coord and distToMid from pixel coordinate.
*/
void
game::commitPosition(Actor& actor) {
    actor.tileX = actor.pixelX / TileSize;
    actor.tileY = actor.pixelY / TileSize;
    actor.distToMidX = TileMidX - actor.pixelX % TileSize;
    actor.distToMidY = TileMidY - actor.pixelY % TileSize;
}

//------------------------------------------------------------------------------
/**
 Test if movement in a given direction is possible.
 FIXME: take cornering into account.
*/
bool
game::canMove(const Actor& actor, Direction dir, bool allowCornering) {
    int distToMid0 = 0;
    int distToMid1 = 0;
    int dirX = 0;
    int dirY = 0;
    if (Up == dir) dirY = -1;
    else if (Down == dir) dirY = +1;
    else if (Left == dir) dirX = -1;
    else if (Right == dir) dirX = +1;
    if (dirY != 0) {
        distToMid0 = actor.distToMidX;
        distToMid1 = actor.distToMidY;
    }
    else {
        distToMid0 = actor.distToMidY;
        distToMid1 = actor.distToMidX;
    }
    
    bool isWall = Wall == this->tile(actor.tileX + dirX, actor.tileY + dirY);
    if ((!allowCornering && 0 != distToMid0) || (0 == distToMid1 && isWall)) {
        // way is blocked
        return false;
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
    const bool allowCornering = (Pacman == actor.actorType);
    if ((NoDirection != dir) && this->canMove(actor, dir, allowCornering)) {
        // can move in intended direction, update actor direction and movement vector
        actor.dir = dir;
        switch (actor.dir) {
            case Left:  actor.dirX = -1; actor.dirY = 0; break;
            case Right: actor.dirX = +1; actor.dirY = 0; break;
            case Up:    actor.dirX = 0; actor.dirY = -1; break;
            case Down:  actor.dirX = 0; actor.dirY = +1; break;
            default:    actor.dirX = 0; actor.dirY = 0; break;
        }
    }
    else {
        // movement into intended direction is not possible, check
        // whether movement in current direction is possible
        if (!this->canMove(actor, actor.dir, allowCornering)) {
            // nope, emergency stop
            actor.dirX = 0;
            actor.dirY = 0;
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
    switch (this->tile(this->actors[Pacman].tileX, this->actors[Pacman].tileY)) {
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
    int x = this->actors[Pacman].tileX;
    int y = this->actors[Pacman].tileY;
    this->setTile(x, y, Empty);
    canvas->SetTile(Sheet::Space, x, y);
}

//------------------------------------------------------------------------------
void
game::eatEnergizer(canvas* canvas) {
    // FIXME!
}

} // namespace Paclone
