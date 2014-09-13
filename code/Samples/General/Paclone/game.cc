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
    "0UUUUUUUUUUUU45UUUUUUUUUUUU1" // 3
    "L............rl............R" // 4
    "L.ebbf.ebbbf.rl.ebbbf.ebbf.R" // 5
    "L r  l.r   l.rl.r   l.r  l R" // 6
    "L.guuh.guuuh.gh.guuuh.guuh.R" // 7
    "L..........................R" // 8
    "L.ebbf.ef.ebbbbbbf.ef.ebbf.R" // 9
    "L.guuh.rl.guuyxuuh.rl.guuh.R" // 10
    "L......rl....rl....rl......R" // 11
    "2BBBBf.rzbbf rl ebbwl.eBBBB3" // 12
    "     L.rxuuh gh guuyl.R     " // 13
    "     L.rl          rl.R     " // 14
    "     L.rl eb####bf rl.R     " // 15
    "UUUUUh.gh r      l gh.gUUUUU" // 16
    "      .   r      l   .      " // 17
    "BBBBBf.ef r      l ef.eBBBBB" // 18
    "     L.rl guuuuuuh rl.R     " // 19
    "     L.rl          rl.R     " // 20
    "     L.rl ebbbbbbf rl.R     " // 21
    "0UUUUh.gh guuyxuuh gh.gUUUU1" // 22
    "L............rl............R" // 23
    "L.ebbf.ebbbf.rl.ebbbf.ebbf.R" // 24
    "L.guyl.guuuh.gh.guuuh.rxuh.R" // 25
    "L ..rl.......  .......rl.. R" // 26
    "6bf.rl.ef.ebbbbbbf.ef.rl.eb8" // 27
    "7uh.gh.rl.guuyxuuh.rl.gh.gu9" // 28
    "L......rl....rl....rl......R" // 29
    "L.ebbbbwzbbf.rl.ebbwzbbbbf.R" // 30
    "L.guuuuuuuuh.gh.guuuuuuuuh.R" // 31
    "L..........................R" // 32
    "2BBBBBBBBBBBBBBBBBBBBBBBBBB3" // 33
    "                            " // 34
    "                            ";// 35

const game::Direction game::reverseDir[NumDirections] = {
    NoDirection, Right, Left, Down, Up,
};

const Sheet::SpriteId game::hollowSpriteMap[NumDirections] = {
    Sheet::EyesLeft, Sheet::EyesLeft, Sheet::EyesRight, Sheet::EyesUp, Sheet::EyesDown
};

const Oryol::int16 game::dirVec[NumDirections][2] = {
    { 0, 0 }, { -1, 0 }, { +1, 0 }, { 0, -1 }, { 0, +1 }
};

game::TileType game::tileMap[Height][Width] = { {Empty} };

const Sheet::SpriteId game::defaultSpriteMap[NumActorTypes][NumDirections] = {
        { Sheet::BlinkyNoDir, Sheet::BlinkyLeft, Sheet::BlinkyRight, Sheet::BlinkyUp, Sheet::BlinkyDown },
        { Sheet::PinkyNoDir, Sheet::PinkyLeft, Sheet::PinkyRight, Sheet::PinkyUp, Sheet::PinkyDown },
        { Sheet::InkyNoDir, Sheet::InkyLeft, Sheet::InkyRight, Sheet::InkyUp, Sheet::InkyDown },
        { Sheet::ClydeNoDir, Sheet::ClydeLeft, Sheet::ClydeRight, Sheet::ClydeUp, Sheet::ClydeDown },
        { Sheet::PacmanNoDir, Sheet::PacmanLeft, Sheet::PacmanRight, Sheet::PacmanUp, Sheet::PacmanDown },
};

//------------------------------------------------------------------------------
game::game() {
    o_assert(std::strlen(charMap) == Width * Height);
}

//------------------------------------------------------------------------------
void
game::Init(canvas* canvas) {
    o_assert(canvas);
    canvas->CopyCharMap(0, 0, Width, Height, charMap);
    this->gameTick = 0;
    this->setupTiles();
    this->setupActors();
    this->setupEnergizers();
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
    this->handleCollide(canvas);
    this->drawEnergizers(canvas);
    this->drawActors(canvas);
    this->gameTick++;
}

//------------------------------------------------------------------------------
void
game::setupTiles() {
    for (int16 y = 0; y < Height; y++) {
        for (int16 x = 0; x < Width; x++) {
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
game::setupEnergizers() {
    static const int16 pos[NumEnergizers][2] = {
        { 1, 6 }, { 26, 6 }, { 1, 26 }, { 26, 26 },
    };
    for (int i = 0; i < NumEnergizers; i++) {
        this->energizers[i].tileX = pos[i][0];
        this->energizers[i].tileY = pos[i][1];
        this->energizers[i].spriteIndex = i;
        this->energizers[i].active = true;
    }
}

//------------------------------------------------------------------------------
void
game::setupActors() {
    static const int16 pos[NumActorTypes][2] = {
        { 13, 14 }, { 13, 14 }, { 13, 14 }, { 13, 14 }, { 13, 26 },
    };
    for (int type = 0; type < NumActorTypes; type++) {
        Actor& actor = this->actors[type];
        actor.type = (ActorType) type;
        actor.spriteIndex = type + NumEnergizers;
        actor.dir = NoDirection;
        actor.nextDir = NoDirection;
        actor.pixelX = pos[type][0] * TileSize + TileMidX + TileSize/2;
        actor.pixelY = pos[type][1] * TileSize + TileMidY;
        actor.state = Scatter;
        actor.frightenedTick = 0;
        commitPosition(actor);
    }
}

//------------------------------------------------------------------------------
void
game::drawActors(canvas* canvas) const {
    o_assert_dbg(canvas);
    for (const Actor& actor : this->actors) {
        Direction dir = (actor.type == Pacman ? actor.dir : actor.nextDir);
        Sheet::SpriteId spriteId = Sheet::InvalidSprite;
        if (actor.state == Frightened) {
            spriteId = Sheet::FrightenedGhost;
            if ((actor.frightenedTick < (2 * 60)) && ((actor.frightenedTick / 15) % 2)) {
                spriteId = Sheet::FlashingGhost;
            }
        }
        else if (actor.state == Hollow) {
            spriteId = this->hollowSpriteMap[dir];
        }
        else {
            spriteId = this->defaultSpriteMap[actor.type][dir];
        }
        if (Sheet::InvalidSprite != spriteId) {
            const int pixX = actor.pixelX - TileMidX - TileSize/2;
            const int pixY = actor.pixelY - TileMidY - TileSize/2;
            const int pixW = 2 * 8;
            const int pixH = 2 * 8;
            canvas->SetSprite(actor.spriteIndex, spriteId, pixX, pixY, pixW, pixH, actor.moveTick / 3);
        }
    }
}

//------------------------------------------------------------------------------
void
game::drawEnergizers(canvas* canvas) const {
    for (const Energizer& cur : this->energizers) {
        if (cur.active) {
            const int pixX = cur.tileX * TileSize - TileSize/2;
            const int pixY = cur.tileY * TileSize - TileSize/2;
            const int pixW = 2 * 8;
            const int pixH = 2 * 8;
            canvas->SetSprite(cur.spriteIndex, Sheet::Energizer, pixX, pixY, pixW, pixH, 0);
        }
        else {
            canvas->ClearSprite(cur.spriteIndex);
        }
    }
}

//------------------------------------------------------------------------------
void
game::updateActors(Direction input) {
    for (Actor& actor : this->actors) {
        if (Pacman == actor.type) {
            // update pacman from user input
            this->computeMove(actor, input);
            this->move(actor);
        }
        else {
            // update ghosts
            bool doMove  = true;
            int numMoves = 1;
            this->updateGhostState(actor);
            switch (actor.state) {
                case Scatter:
                    this->chooseScatterTarget(actor);
                    break;
                case Chase:
                    this->chooseChaseTarget(actor);
                    break;
                case Frightened:
                    this->chooseFrightenedTarget(actor);
                    doMove = this->gameTick & 1;
                    break;
                case Hollow:
                    this->chooseHollowTarget(actor);
                    numMoves = 2;
                    break;
                default:
                    break;
            }
            if (doMove) {
                for (int i = 0; i < numMoves; i++) {
                    this->updateDirection(actor);
                    this->computeMove(actor, actor.dir);
                    this->move(actor);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
void
game::updateGhostState(Actor& ghost) {

    GhostState newState = ghost.state;
    if (ghost.state == Hollow) {
        if ((ghost.tileX == ghost.targetX) && (ghost.tileY == ghost.targetY)) {
            // FIXME: switch state to House
            newState = Scatter;
            ghost.frightenedTick = 0;
        }
    }
    else if (ghost.frightenedTick > 0) {
        newState = Frightened;
        ghost.frightenedTick--;
    }
    else {
        const int scatterFrames = 7 * 60;
        const int chaseFrames = 20 * 60;
        int tick = this->gameTick % (scatterFrames + chaseFrames);
        if (tick >= scatterFrames) {
            newState = Chase;
        }
        else {
            newState = Scatter;
        }
    }
    if (ghost.state != newState) {
        // invert current direction when switching state
        ghost.nextDir = game::reverseDir[ghost.dir];
        ghost.state = newState;
    }
}

//------------------------------------------------------------------------------
void
game::chooseScatterTarget(Actor& ghost) const {
    static const Oryol::int16 scatterTargets[NumActorTypes][2] = {
        { 25, 0 }, { 2, 0 }, {27, 34}, { 0, 34}, { 0, 0 },
    };
    ghost.targetX = scatterTargets[ghost.type][0];
    ghost.targetY = scatterTargets[ghost.type][1];
}

//------------------------------------------------------------------------------
void
game::chooseChaseTarget(Actor& ghost) const {
    const Actor& pacman = this->actors[Pacman];
    if (Blinky == ghost.type) {
        // directly target pacman
        ghost.targetX = pacman.tileX;
        ghost.targetY = pacman.tileY;
    }
    else if (Pinky == ghost.type) {
        // target 4 tiles ahead of pacman
        // this does not reproduce the 'diagonal up' overflow
        ghost.targetX = pacman.tileX + 4 * game::dirVec[pacman.dir][0];
        ghost.targetY = pacman.tileY + 4 * game::dirVec[pacman.dir][1];
    }
    else if (Inky == ghost.type) {
        const int16 mx = pacman.tileX + 2 * game::dirVec[pacman.dir][0];
        const int16 my = pacman.tileY + 2 * game::dirVec[pacman.dir][1];
        const Actor& blinky = this->actors[Blinky];
        const int16 dx = mx - blinky.tileX;
        const int16 dy = my - blinky.tileY;
        ghost.targetX = blinky.tileX + 2 * dx;
        ghost.targetY = blinky.tileY + 2 * dy;
    }
    else if (Clyde == ghost.type) {
        const int16 dx = ghost.tileX - pacman.tileX;
        const int16 dy = ghost.tileY - pacman.tileY;
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
void
game::chooseFrightenedTarget(Actor& ghost) const {
    // simply choose a random target point now and then
    if (this->gameTick % 13) {
        ghost.targetX = rand() % Width;
        ghost.targetY = rand() % Height;
    }
}

//------------------------------------------------------------------------------
void
game::chooseHollowTarget(Actor& ghost) const {
    ghost.targetX = 13;
    ghost.targetY = 14;
}

//------------------------------------------------------------------------------
void
game::updateDirection(Actor& ghost) const {
    if (((ghost.distToMidX == 0) && (ghost.distToMidY == 0)) || (NoDirection == ghost.dir)) {
        if (NoDirection != ghost.nextDir) {
            ghost.dir = ghost.nextDir;
        }
        
        // look ahead one tile to find next direction
        const int16 x = ghost.tileX + game::dirVec[ghost.dir][0];
        const int16 y = ghost.tileY + game::dirVec[ghost.dir][1];
        
        // now test distance of each possible target tile
        // note: directions have a preference in case of a tie:
        // UP -> LEFT -> DOWN -> RIGHT
        Direction bestDirection = NoDirection;
        int32 minDist = 1000000;
        int32 dist;
        const Direction dirs[4] = { Up, Left, Down, Right };
        for (Direction dir : dirs) {
            const Direction revDir = game::reverseDir[dir];
            const int16 checkX = x + game::dirVec[dir][0];
            const int16 checkY = y + game::dirVec[dir][1];
            if ((revDir != ghost.dir) && (Wall != this->tile(checkX, checkY))) {
                if ((dist = targetDistSq(checkX, checkY, ghost.targetX, ghost.targetY)) < minDist) {
                    bestDirection = dir;
                    minDist = dist;
                }
            }
        }
        ghost.nextDir = bestDirection;
    }
}

//------------------------------------------------------------------------------
void
game::move(Actor& actor) const {
    const int16 dirX = game::dirVec[actor.dir][0];
    const int16 dirY = game::dirVec[actor.dir][1];
    actor.pixelX += dirX;
    actor.pixelY += dirY;
    if (actor.dir != NoDirection) {
        actor.moveTick++;
    }

    // cornering (drag actor towards middle line)
    if (dirX != 0) {
        if (actor.distToMidY < 0)       actor.pixelY--;
        else if (actor.distToMidY > 0)  actor.pixelY++;
    }
    else if (dirY != 0) {
        if (actor.distToMidX < 0)       actor.pixelX--;
        else if (actor.distToMidX > 0)  actor.pixelX++;
    }
    
    // wrap around X for teleport (not 100% right I think)
    if (actor.pixelX < 0) actor.pixelX = Width * TileSize - 1;
    else if (actor.pixelX > Width * TileSize) actor.pixelX = 0;
    
    commitPosition(actor);
}

//------------------------------------------------------------------------------
/**
 Test if movement in a given direction is possible.
 FIXME: take cornering into account.
*/
bool
game::canMove(const Actor& actor, Direction dir, bool allowCornering) const {
    const int dirX = game::dirVec[dir][0];
    const int dirY = game::dirVec[dir][1];
    int distToMid0, distToMid1;
    if (dirY != 0) {
        distToMid0 = actor.distToMidX;
        distToMid1 = actor.distToMidY;
    }
    else {
        distToMid0 = actor.distToMidY;
        distToMid1 = actor.distToMidX;
    }
    
    bool isWall = Wall == this->tile(actor.tileX + dirX, actor.tileY + dirY);
    if ((!allowCornering && (0 != distToMid0)) || (isWall && (0 == distToMid1))) {
        // way is blocked
        return false;
    }
    else {
        // way is free
        return true;
    }
}

//------------------------------------------------------------------------------
/**
 Movement rules for Pacman and Ghosts are different: see 'Cornering':
 http://home.comcast.net/~jpittman2/pacman/pacmandossier.html#CH2_Cornering
*/
void
game::computeMove(Actor& actor, Direction dir) const {

    // dir is the direction the actor *wants* to move,
    // which might not be possible
    const bool allowCornering = (Pacman == actor.type);
    if ((NoDirection != dir) && this->canMove(actor, dir, allowCornering)) {
        // can move in intended direction, update actor direction and movement vector
        actor.dir = dir;
    }
    else {
        // movement into intended direction is not possible, check
        // whether movement in current direction is possible
        if (!this->canMove(actor, actor.dir, allowCornering)) {
            // nope, emergency stop
            actor.dir = NoDirection;
        }
    }
}

//------------------------------------------------------------------------------
/**
    Checks for collission between pacman and ghosts, and between pacman and
    pills.
*/
void
game::handleCollide(canvas* canvas) {
    const int16 pacX = this->actors[Pacman].tileX;
    const int16 pacY = this->actors[Pacman].tileY;

    // check for energizer overlap
    for (Energizer& enrg : this->energizers) {
        if (enrg.active && (pacX == enrg.tileX) && (pacY == enrg.tileY)) {
            this->eatEnergizer(enrg);
        }
    }
    
    // check for ghost overlap
    for (Actor& actor : this->actors) {
        if (Pacman != actor.type) {
            if ((pacX == actor.tileX) && (pacY == actor.tileY)) {
                switch (actor.state) {
                    case Scatter:
                    case Chase:
                        // Pacman dies
                        this->killPacman();
                        break;
                    case Frightened:
                        // Ghost dies
                        this->killGhost(actor);
                        break;
                    default:
                        break;
                }
            }
        }
    }
    
    // check for pill
    if (this->tile(pacX, pacY) == Pill) {
        this->eatPill(canvas);
    }
}

//------------------------------------------------------------------------------
void
game::eatPill(canvas* canvas) {
    const int16 x = this->actors[Pacman].tileX;
    const int16 y = this->actors[Pacman].tileY;
    this->setTile(x, y, Empty);
    canvas->SetTile(Sheet::Space, x, y);
}

//------------------------------------------------------------------------------
void
game::eatEnergizer(Energizer& energizer) {
    energizer.active = false;
    for (Actor& actor : this->actors) {
        if (Pacman != actor.type) {
            actor.frightenedTick = 6 * 60;
        }
    }
}

//------------------------------------------------------------------------------
void
game::killPacman() {
    Log::Info("PACMAN KILLED!\n");
}

//------------------------------------------------------------------------------
void
game::killGhost(Actor& ghost) {
    ghost.state = Hollow;
}

} // namespace Paclone
