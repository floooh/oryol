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
    "     L.rl mjj--jjn rl.R     " // 15
    "UUUUUh.gh i      i gh.gUUUUU" // 16
    "      .   i      i   .      " // 17
    "BBBBBf.ef i      i ef.eBBBBB" // 18
    "     L.rl ojjjjjjp rl.R     " // 19
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

// map from a direction into the reverse direction
const game::Direction game::reverseDir[NumDirections] = {
    NoDirection, Right, Left, Down, Up,
};

// map direction enum to 2D movement vec
const Oryol::int16 game::dirVec[NumDirections][2] = {
    { 0, 0 }, { -1, 0 }, { +1, 0 }, { 0, -1 }, { 0, +1 }
};

// actor home positions/targets
const int16 game::homeTilePos[NumActorTypes][2] = {
    { 13, 14 }, { 13, 17 }, { 11, 17 }, { 15, 17 }, { 13, 26 },
};

game::TileType game::tileMap[Height][Width] = { {Empty} };

// map actor types and movement direction to sprites
const Sheet::SpriteId game::defaultSpriteMap[NumActorTypes][NumDirections] = {
        { Sheet::BlinkyNoDir, Sheet::BlinkyLeft, Sheet::BlinkyRight, Sheet::BlinkyUp, Sheet::BlinkyDown },
        { Sheet::PinkyNoDir, Sheet::PinkyLeft, Sheet::PinkyRight, Sheet::PinkyUp, Sheet::PinkyDown },
        { Sheet::InkyNoDir, Sheet::InkyLeft, Sheet::InkyRight, Sheet::InkyUp, Sheet::InkyDown },
        { Sheet::ClydeNoDir, Sheet::ClydeLeft, Sheet::ClydeRight, Sheet::ClydeUp, Sheet::ClydeDown },
        { Sheet::PacmanNoDir, Sheet::PacmanLeft, Sheet::PacmanRight, Sheet::PacmanUp, Sheet::PacmanDown },
};

// map from direction to sprite for ghost hollow mode
const Sheet::SpriteId game::hollowSpriteMap[NumDirections] = {
    Sheet::EyesLeft, Sheet::EyesLeft, Sheet::EyesRight, Sheet::EyesUp, Sheet::EyesDown
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
    this->dotCounter = 0;
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
                case '.': tileType = Dot; break;
                case ' ': tileType = Empty; break;
                case '-': tileType = Door; break;
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
    static const Direction startDir[NumActorTypes] = {
        Left, Down, Up, Up, NoDirection
    };
    static const int dotLimits[NumActorTypes] = {
        0, 0, 30, 60, 0
    };
    
    for (int type = 0; type < NumActorTypes; type++) {
        Actor& actor = this->actors[type];
        actor.type = (ActorType) type;
        actor.spriteIndex = type + NumEnergizers;
        actor.dir = startDir[type];
        actor.nextDir = actor.dir;
        game::homePixelPos(game::homeTilePos[type][0], game::homeTilePos[type][1], actor.pixelX, actor.pixelY);
        actor.homeBasePixelX = actor.pixelX;
        actor.homeBasePixelY = actor.pixelY;
        actor.state = type == Blinky ? Scatter : House;
        actor.frightenedTick = 0;
        actor.dotCounter = 0;
        actor.dotLimit = dotLimits[type];
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
        else if ((actor.state == Hollow) || (actor.state == EnterHouse)) {
            spriteId = game::hollowSpriteMap[dir];
        }
        else {
            spriteId = game::defaultSpriteMap[actor.type][dir];
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
        bool doMove = true;
        if (Pacman == actor.type) {
            // update pacman from user input
            if (doMove) {
                this->computeMove(actor, input, true);
                this->move(actor, true);
            }
        }
        else {
            // update ghosts
            this->updateGhostState(actor);
            if (House == actor.state) {
                // special behaviour when inside house
                if (doMove) {
                    this->updateHouseDirection(actor);
                    this->move(actor, false);
                }
            }
            else if (EnterHouse == actor.state) {
                if (doMove) {
                    this->updateEnterHouseDirection(actor);
                    this->move(actor, false);
                }
            }
            else if (LeaveHouse == actor.state) {
                // special behaviour when leaving house
                if (doMove) {
                    this->updateLeaveHouseDirection(actor);
                    this->move(actor, false);
                }
            }
            else {
                // all other states have the same movement
                // behaviour and only differ in choosing the target
                bool allowCornering = false;
                int numMove = 1;
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
                        numMove = this->gameTick & 1 ? 1 : 2;
                        break;
                    default:
                        break;
                }
                if (doMove) {
                    for (int i = 0; i < numMove; i++) {
                        this->updateGhostDirection(actor);
                        this->computeMove(actor, actor.dir, allowCornering);
                        this->move(actor, true);
                    }
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
        // hollow is moving at 2x speed, so need to relax check in X
        if (game::nearEqual(ghost.pixelX, AntePortasPixelX, 8) && (ghost.pixelY == AntePortasPixelY)) {
            // let EnterHouse state take over, which moves the hollow ghost
            // to its homebase position
            newState = EnterHouse;
            ghost.pixelX = AntePortasPixelX;
            ghost.pixelY = AntePortasPixelY;
            ghost.frightenedTick = 0;
            ghost.dotCounter = 0;
        }
    }
    else if (ghost.state == EnterHouse) {
        if ((ghost.pixelX == ghost.homeBasePixelX) && (ghost.pixelY == ghost.homeBasePixelY)) {
            newState = House;
        }
    }
    else if (ghost.state == House) {
        // check if ghost may leave the house
        // FIXME: this is more involved then just the dot counter!!!
        if (ghost.dotCounter >= ghost.dotLimit) {
            newState = LeaveHouse;
        }
        
    }
    else if (ghost.state == LeaveHouse) {
        if ((ghost.pixelX == AntePortasPixelX) && (ghost.pixelY == AntePortasPixelY)) {
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
        // switch state and handle initial state direction
        // (usually the direction is inverted, except in some special states)
        if (LeaveHouse == ghost.state) {
            ghost.nextDir = Left;
        }
        else if (EnterHouse == ghost.state) {
            ghost.nextDir = Down;
        }
        else {
            ghost.nextDir = reverseDir[ghost.dir];
        }
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
    ghost.targetY = 15;
}

//------------------------------------------------------------------------------
void
game::updateGhostDirection(Actor& ghost) const {
    // this method is not valid for ghosts in House state
    o_assert_dbg(House != ghost.state);

    // only compute new direction when currently at the mid of a tile or not currently moving
    if (((ghost.distToMidX == 0) && (ghost.distToMidY == 0)) || (NoDirection == ghost.dir)) {
        
        if (NoDirection != ghost.nextDir) {
            ghost.dir = ghost.nextDir;
        }
        
        // one-tile-ahead position
        const int16 nextX = ghost.tileX + game::dirVec[ghost.dir][0];
        const int16 nextY = ghost.tileY + game::dirVec[ghost.dir][1];
        
        // this is the default behaviour:
        // now test distance of each possible target tile
        // note: directions have a preference in case of a tie:
        // UP -> LEFT -> DOWN -> RIGHT
        Direction bestDirection = NoDirection;
        int32 minDist = 1000000;
        int32 dist;
        const Direction dirs[4] = { Up, Left, Down, Right };
        for (Direction dir : dirs) {
            const Direction revDir = game::reverseDir[dir];
            const int16 checkX = nextX + game::dirVec[dir][0];
            const int16 checkY = nextY + game::dirVec[dir][1];
            if ((revDir != ghost.dir) && !this->isBlocked(ghost, checkX, checkY)) {
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
game::updateHouseDirection(Actor& ghost) const {
    if ((ghost.dir != Up) && (ghost.dir != Down)) {
        ghost.dir = Up;
    }
    const int16 nextX = ghost.tileX + game::dirVec[ghost.dir][0];
    const int16 nextY = ghost.tileY + game::dirVec[ghost.dir][1];
    if (this->isBlocked(ghost, nextX, nextY)) {
        ghost.dir = reverseDir[ghost.dir];
        ghost.nextDir = ghost.dir;
    }
}

//------------------------------------------------------------------------------
void
game::updateLeaveHouseDirection(Actor& ghost) const {
    // first move to the middle, then up, all by pixel coords
    if (ghost.pixelX > AntePortasPixelX) {
        ghost.dir = Left;
    }
    else if (ghost.pixelX < AntePortasPixelX) {
        ghost.dir = Right;
    }
    else {
        ghost.dir = Up;
    }
    ghost.nextDir = ghost.dir;
}

//------------------------------------------------------------------------------
void
game::updateEnterHouseDirection(Actor& ghost) const {
    // at the begging, we're on the home-lane, first move
    // toward the right X position in front of the door,
    // then down into the house, and then to the homeBase position
    if ((ghost.pixelY == AntePortasPixelY) && (ghost.pixelX > AntePortasPixelX)) {
        ghost.dir = Left;
    }
    else if ((ghost.pixelY == AntePortasPixelY) && (ghost.pixelX < AntePortasPixelX)) {
        ghost.dir = Right;
    }
    else if (ghost.pixelY < ghost.homeBasePixelY) {
        ghost.dir = Down;
    }
    else if (ghost.pixelX > ghost.homeBasePixelX) {
        ghost.dir = Left;
    }
    else if (ghost.pixelX < ghost.homeBasePixelX) {
        ghost.dir = Right;
    }
    else {
        ghost.dir = NoDirection;
    }
}

//------------------------------------------------------------------------------
void
game::move(Actor& actor, bool allowCornering) const {
    const int16 dirX = game::dirVec[actor.dir][0];
    const int16 dirY = game::dirVec[actor.dir][1];
    actor.pixelX += dirX;
    actor.pixelY += dirY;
    if (actor.dir != NoDirection) {
        actor.moveTick++;
    }

    // cornering (drag actor towards middle line), this is switched off
    // in House mode
    if (allowCornering) {
        if (dirX != 0) {
            if (actor.distToMidY < 0)       actor.pixelY--;
            else if (actor.distToMidY > 0)  actor.pixelY++;
        }
        else if (dirY != 0) {
            if (actor.distToMidX < 0)       actor.pixelX--;
            else if (actor.distToMidX > 0)  actor.pixelX++;
        }
    }
    
    // wrap around X for teleport (not 100% right I think)
    if (actor.pixelX < 0) actor.pixelX = Width * TileSize - 1;
    else if (actor.pixelX > Width * TileSize) actor.pixelX = 0;
    
    commitPosition(actor);
}

//------------------------------------------------------------------------------
/**
 Check if a tile is blocked for actor at specific position.
*/
bool
game::isBlocked(const Actor& actor, int16 tileX, int16 tileY) const {
    TileType tile = this->tile(tileX, tileY);
    if ((Hollow == actor.state) || (LeaveHouse == actor.state)) {
        return (Wall == tile);
    }
    else {
        return (Wall == tile) || (Door == tile);
    }
}

//------------------------------------------------------------------------------
/**
 Test if movement in a given direction is possible.
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
    
    bool isBlocked = this->isBlocked(actor, actor.tileX + dirX, actor.tileY + dirY);
    if ((!allowCornering && (0 != distToMid0)) || (isBlocked && (0 == distToMid1))) {
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
game::computeMove(Actor& actor, Direction dir, bool allowCornering) const {

    // dir is the direction the actor *wants* to move,
    // which might not be possible
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
    dots.
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
    
    // check for dot
    if (this->tile(pacX, pacY) == Dot) {
        this->eatDot(canvas);
    }
}

//------------------------------------------------------------------------------
void
game::updateDotCounters() {

    // Blinky's dot counter is never updated since it's
    // limit is always 0, the other ghost's dot counters
    // are updated for the first ghost in the house in
    // the following order:
    static ActorType ghosts[] = { Pinky, Inky, Clyde };
    for (ActorType type : ghosts) {
        if (House == this->actors[type].state) {
            this->actors[type].dotCounter++;
            break;
        }
    }
    
    
}

//------------------------------------------------------------------------------
void
game::eatDot(canvas* canvas) {
    const int16 x = this->actors[Pacman].tileX;
    const int16 y = this->actors[Pacman].tileY;
    this->setTile(x, y, Empty);
    canvas->SetTile(Sheet::Space, x, y);
    this->updateDotCounters();
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
