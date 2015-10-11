//------------------------------------------------------------------------------
//  game.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "game.h"
#include "draw.h"
#include <cstring>

using namespace Oryol;

namespace Paclone {

//------------------------------------------------------------------------------
game::game() {
    o_assert(std::strlen(state.charMap) == Width * Height);
}

//------------------------------------------------------------------------------
void
game::Init(canvas* canvas, sound* sound) {
    state.gameTick = 0;
    state.roundCounter = 0;
    state.score = 0;
    state.hiscore = 0;
    state.lives = NumLives;
    state.blockCounter = 0;
    state.dotCounter = 0;
    state.noDotFrames = 0;
    state.ghostKillCounter = 0;

    this->startNewRound(canvas, sound);
}

//------------------------------------------------------------------------------
void
game::Cleanup() {
    // FIXME!
}

//------------------------------------------------------------------------------
const Int2&
game::PacmanPos() const {
    return this->state.actors[Pacman].pixelPos;
}

//------------------------------------------------------------------------------
void
game::Update(int tick, canvas* canvas, sound* sound, Direction input) {
    if (this->checkNewRound()) {
        this->startNewRound(canvas, sound);
    }
    if (this->checkLifeLost()) {
        this->startNewLife(sound);
    }
    this->updateCounters();
    if (state.blockCounter == 0) {
        this->updateActors(input);
        this->updateGhostSounds(sound);
        this->handleCollide(canvas, sound);
        state.gameTick++;
        state.noDotFrames++;
    }
    draw::drawEnergizers(state, canvas);
    draw::drawActors(state, canvas);
    draw::drawChrome(state, canvas);
}

//------------------------------------------------------------------------------
void
game::updateCounters() {
    if (state.blockCounter > 0) {
        state.blockCounter--;
    }
    for (Actor& actor : state.actors) {
        if (actor.killedTicks > 0) {
            actor.killedTicks--;
        }
    }
}

//------------------------------------------------------------------------------
void
game::initTiles() {
    for (int y = 0; y < Height; y++) {
        for (int x = 0; x < Width; x++) {
            char c = state.charMap[y * Width + x];
            TileType tileType = Empty;
            switch (c) {
                case '.': tileType = Dot; break;
                case ' ': tileType = Empty; break;
                case '-': tileType = Door; break;
                default:  tileType = Wall; break;
            }
            this->setTile(Int2{x,y}, tileType);
        }
    }
}

//------------------------------------------------------------------------------
void
game::initEnergizers() {
    for (int i = 0; i < NumEnergizers; i++) {
        state.energizers[i].tilePos = state.energizerPos[i];
        state.energizers[i].spriteIndex = i;
        state.energizers[i].active = true;
    }
}

//------------------------------------------------------------------------------
void
game::initActors() {
    
    // FIXME: split into level init and round init!
    
    // map actor type to sprite index, pacman needs to render
    // after energizers and before ghosts
    int spriteIndices[NumActorTypes] = {
        NumEnergizers + 1,  // Blinky
        NumEnergizers + 2,  // Pinky
        NumEnergizers + 3,  // Inky
        NumEnergizers + 4,  // Clyde
        NumEnergizers + 0,  // Pacman
    };
    
    for (int type = 0; type < NumActorTypes; type++) {
        Actor& actor = state.actors[type];
        actor.type = (ActorType) type;
        actor.spriteIndex = spriteIndices[type];
        actor.dir = state.actorStartDir[type];
        actor.nextDir = actor.dir;
        actor.pixelPos = func::homePixelPos(state.homeTilePos[type]);
        actor.homeBasePixelPos = actor.pixelPos;
        actor.state = type == Blinky ? Scatter : House;
        actor.frightenedTick = 0;
        actor.dotCounter = 0;
        actor.dotLimit = state.actorDotLimits[type];
        actor.forceLeaveHouse = false;
        actor.killedCount = 0;
        actor.killedTicks = 0;
        commitPosition(actor);
    }
}

//------------------------------------------------------------------------------
bool
game::checkNewRound() const {
    // a new round starts when all dots have been eaten
    return 240 == state.dotCounter;
}

//------------------------------------------------------------------------------
void
game::startNewRound(canvas* canvas, sound* sound) {
    o_assert_dbg(sound);

    state.blockCounter = sound::IntroSongLengthTicks;
    state.dotCounter = 0;
    state.noDotFrames = 0;
    state.ghostKillCounter = 0;

    draw::initMap(state, canvas);
    this->initTiles();
    this->initActors();
    this->initEnergizers();
    sound->Reset();
    sound->IntroSong();
}

//------------------------------------------------------------------------------
bool
game::checkLifeLost() const {
    return (1 == state.actors[Pacman].killedTicks);
}

//------------------------------------------------------------------------------
void
game::startNewLife(sound* sound) {
    o_assert_dbg(this->checkLifeLost());

    // pacman has lost a life, start new life
    state.lives--;
    sound->Reset();
    if (state.lives <= 0) {
        // no more lifes left
        state.lives = 0;
    }
    state.gameTick = 0;
    state.blockCounter = 180;
    state.noDotFrames = 0;
    state.ghostKillCounter = 0;
    for (Actor& actor : state.actors) {
        actor.dir = state.actorStartDir[actor.type];
        actor.nextDir = actor.dir;
        actor.pixelPos = func::homePixelPos(state.homeTilePos[actor.type]);
        actor.state = actor.type == Blinky ? Scatter : House;
        actor.frightenedTick = 0;
        actor.dotCounter = 0;
        actor.forceLeaveHouse = false;
        if (Pacman != actor.type) {
            actor.killedCount = 0;
        }
        actor.killedTicks = 0;
        commitPosition(actor);
    }
}

//------------------------------------------------------------------------------
void
game::updateActors(Direction input) {
    
    // first check if a ghost is forced out of the
    // monster den because Pacman hasn't been eating dots for too long,
    // only the first monster will be evicted and the counter reset to 0
    // (it is also reset every time a dot is eaten)
    for (Actor& actor : state.actors) {
        if (House == actor.state) {
            if (state.noDotFrames > 4 * 60) {
                actor.forceLeaveHouse = true;
                state.noDotFrames = 0;
                break;
            }
        }
    }
    
    for (Actor& actor : state.actors) {
        if (Pacman == actor.type) {
            // update pacman from user input
            for (int i = 0, num = func::actorSpeed(actor, state.gameTick); i < num; i++) {
                actor.dir = func::checkDir(actor, input, true);
                this->move(actor, true);
            }
        }
        else {
            // update ghosts
            this->updateGhostState(actor);
            const int numMove = func::actorSpeed(actor, state.gameTick);
            if (House == actor.state) {
                // special behaviour when inside house
                for (int i = 0; i < numMove; i++) {
                    this->updateHouseDirection(actor);
                    this->move(actor, false);
                }
            }
            else if (EnterHouse == actor.state) {
                for (int i = 0; i < numMove; i++) {
                    this->updateEnterHouseDirection(actor);
                    this->move(actor, false);
                }
            }
            else if (LeaveHouse == actor.state) {
                // special behaviour when leaving house
                for (int i = 0; i < numMove; i++) {
                    this->updateLeaveHouseDirection(actor);
                    this->move(actor, false);
                }
            }
            else {
                // all other states have the same movement
                // behaviour and only differ in choosing the target
                switch (actor.state) {
                    case Scatter:
                        actor.targetPos = func::chooseScatterTarget(actor);
                        break;
                    case Chase:
                        actor.targetPos = func::chooseChaseTarget(actor, state.actors[Pacman], state.actors[Blinky]);
                        break;
                    case Frightened:
                        actor.targetPos = func::chooseFrightenedTarget(actor, state.gameTick);
                        break;
                    case Hollow:
                        actor.targetPos = func::chooseHollowTarget(actor);
                        break;
                    default:
                        break;
                }
                for (int i = 0; i < numMove; i++) {
                    this->updateGhostDirection(actor);
                    actor.dir = func::checkDir(actor, actor.dir, false);
                    this->move(actor, true);
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
        if (func::nearEqual(ghost.pixelPos.x, AntePortasPixelX, 8) && (ghost.pixelPos.y == AntePortasPixelY)) {
            // let EnterHouse state take over, which moves the hollow ghost
            // to its homebase position
            newState = EnterHouse;
            ghost.pixelPos = Int2{AntePortasPixelX, AntePortasPixelY};
            ghost.dotCounter = 0;
        }
    }
    else if (ghost.state == EnterHouse) {
        if (func::equal(ghost.pixelPos, ghost.homeBasePixelPos)) {
            newState = House;
        }
    }
    else if (ghost.state == House) {
        // check if ghost may leave the house
        // FIXME: this is more involved then just the dot counter!!!
        if ((ghost.dotCounter >= ghost.dotLimit) || ghost.forceLeaveHouse) {
            newState = LeaveHouse;
        }
        
    }
    else if (ghost.state == LeaveHouse) {
        if (func::equal(ghost.pixelPos, Int2{AntePortasPixelX, AntePortasPixelY})) {
            newState = Scatter;
        }
    }
    else if (ghost.frightenedTick > 0) {
        newState = Frightened;
    }
    else {
        // alternate between scatter and chase
        newState = func::lookupScatterChaseMode(state.gameTick);
    }
    if (ghost.state != newState) {
        // switch state and handle initial state direction
        // (usually the direction is inverted, except in some special states)
        if (LeaveHouse == ghost.state) {
            ghost.dir = Left;
            ghost.nextDir = Left;
        }
        else if (EnterHouse == ghost.state) {
            ghost.nextDir = Down;
        }
        else if (Frightened == ghost.state) {
            // don't reverse direction when leaving frightened state
            // force frightened tick to 0 when leaving frightened state
        }
        else {
            ghost.nextDir = state.reverseDir[ghost.dir];
        }
        ghost.state = newState;
    }
    // decrease frightened tick (in ALL states!)
    if (ghost.frightenedTick > 0) {
        ghost.frightenedTick--;
    }
    ghost.forceLeaveHouse = false;
}

//------------------------------------------------------------------------------
void
game::updateGhostDirection(Actor& ghost) const {
    // note: this is only called in the non-special ghost states
    // (scatter, chase, frightened, hollow)
    o_assert_dbg(Scatter == ghost.state || Chase == ghost.state || Frightened == ghost.state || Hollow == ghost.state);

    // only compute new direction when currently at the mid of a tile or not currently moving
    if (func::equal(ghost.distToMid, Int2{0,0}) || (NoDirection == ghost.dir)) {
        
        if (NoDirection != ghost.nextDir) {
            ghost.dir = ghost.nextDir;
        }
        
        // one-tile-ahead position
        const int nextX = ghost.tilePos.x + state.dirVec[ghost.dir].x;
        const int nextY = ghost.tilePos.y + state.dirVec[ghost.dir].y;
        
        // this is the default behaviour:
        // now test distance of each possible target tile
        // note: directions have a preference in case of a tie:
        // UP -> LEFT -> DOWN -> RIGHT
        Direction bestDirection = NoDirection;
        int minDist = 1000000;
        int dist;
        const Direction dirs[4] = { Up, Left, Down, Right };
        for (Direction dir : dirs) {
            const Direction revDir = state.reverseDir[dir];
            Int2 checkPos{nextX + state.dirVec[dir].x, nextY + state.dirVec[dir].y};
            if ((revDir != ghost.dir) && !func::isBlocked(ghost.state, checkPos)) {
                if ((dist = func::targetDistSq(checkPos, ghost.targetPos)) < minDist) {
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
    o_assert_dbg(House == ghost.state);

    if ((ghost.dir != Up) && (ghost.dir != Down)) {
        ghost.dir = Up;
    }
    Int2 nextPos{
        ghost.tilePos.x + state.dirVec[ghost.dir].x,
        ghost.tilePos.y + state.dirVec[ghost.dir].y};
    if (func::isBlocked(ghost.state, nextPos)) {
        ghost.dir = state.reverseDir[ghost.dir];
        ghost.nextDir = ghost.dir;
    }
}

//------------------------------------------------------------------------------
void
game::updateLeaveHouseDirection(Actor& ghost) const {
    o_assert_dbg(LeaveHouse == ghost.state);

    // first move to the middle, then up, all by pixel coords
    if (ghost.pixelPos.x > AntePortasPixelX) {
        ghost.dir = Left;
    }
    else if (ghost.pixelPos.x < AntePortasPixelX) {
        ghost.dir = Right;
    }
    else if (ghost.pixelPos.y > AntePortasPixelY) {
        ghost.dir = Up;
    }
    else if (ghost.pixelPos.y < AntePortasPixelY) {
        ghost.dir = Down;
    }
    else {
        // exactly on target point
        ghost.dir = Left;
    }
    ghost.nextDir = ghost.dir;
}

//------------------------------------------------------------------------------
void
game::updateEnterHouseDirection(Actor& ghost) const {
    o_assert_dbg(EnterHouse == ghost.state);

    // at the begging, we're on the home-lane, first move
    // toward the right X position in front of the door,
    // then down into the house, and then to the homeBase position
    if ((ghost.pixelPos.y == AntePortasPixelY) && (ghost.pixelPos.x > AntePortasPixelX)) {
        ghost.dir = Left;
    }
    else if ((ghost.pixelPos.y == AntePortasPixelY) && (ghost.pixelPos.x < AntePortasPixelX)) {
        ghost.dir = Right;
    }
    else if (ghost.pixelPos.y < ghost.homeBasePixelPos.y) {
        ghost.dir = Down;
    }
    else if (ghost.pixelPos.x > ghost.homeBasePixelPos.x) {
        ghost.dir = Left;
    }
    else if (ghost.pixelPos.x < ghost.homeBasePixelPos.x) {
        ghost.dir = Right;
    }
    else {
        ghost.dir = Up;
    }
}

//------------------------------------------------------------------------------
void
game::updateGhostSounds(sound* sound) {
    // decide what sound to play
    bool frightened = false;
    bool hollowed = false;
    for (const Actor& actor : state.actors) {
        if (Hollow == actor.state) {
            hollowed = true;
            break;
        }
    }
    if (!hollowed) {
        for (const Actor& actor : state.actors) {
            if (Frightened == actor.state) {
                frightened = true;
                break;
            }
        }
    }

    // play ghost state sound
    if (hollowed) {
        sound->GhostAlarm(state.gameTick);
    }
    else if (frightened) {
        sound->GhostFrightened(state.gameTick);
    }
    else {
        sound->GhostNormal(state.gameTick);
    }
}

//------------------------------------------------------------------------------
void
game::move(Actor& actor, bool allowCornering) const {
    const Int2& dir = state.dirVec[actor.dir];
    actor.pixelPos.x += dir.x;
    actor.pixelPos.y += dir.y;
    if (actor.dir != NoDirection) {
        actor.moveTick++;
    }

    // cornering (drag actor towards middle line), this is switched off
    // in House mode
    if (allowCornering) {
        if (dir.x != 0) {
            if (actor.distToMid.y < 0)       actor.pixelPos.y--;
            else if (actor.distToMid.y > 0)  actor.pixelPos.y++;
        }
        else if (dir.y != 0) {
            if (actor.distToMid.x < 0)       actor.pixelPos.x--;
            else if (actor.distToMid.x > 0)  actor.pixelPos.x++;
        }
    }
    
    // wrap around X for teleport (not 100% right I think)
    if (actor.pixelPos.x < 0) actor.pixelPos.x = Width * TileSize - 1;
    else if (actor.pixelPos.x > Width * TileSize) actor.pixelPos.x = 0;
    
    commitPosition(actor);
}

//------------------------------------------------------------------------------
/**
    Checks for collission between pacman and ghosts, and between pacman and
    dots.
*/
void
game::handleCollide(canvas* canvas, sound* sound) {
    const Int2 pacPos = state.actors[Pacman].tilePos;

    // check for energizer overlap
    for (Energizer& enrg : state.energizers) {
        if (enrg.active && func::equal(pacPos, enrg.tilePos)) {
            this->eatEnergizer(enrg, sound);
        }
    }
    
    // check for ghost overlap
    for (Actor& actor : state.actors) {
        if (Pacman != actor.type) {
            if (func::equal(pacPos, actor.tilePos)) {
                switch (actor.state) {
                    case Scatter:
                    case Chase:
                        // Pacman dies
                        this->killPacman();
                        break;
                    case Frightened:
                        // Ghost dies
                        this->killGhost(actor, sound);
                        break;
                    default:
                        break;
                }
            }
        }
    }
    
    // check for dot
    if (func::tileType(pacPos) == Dot) {
        this->eatDot(canvas, sound);
    }
}

//------------------------------------------------------------------------------
void
game::addScore(int val) {
    state.score += val;
    if (state.score > state.hiscore) {
        state.hiscore = state.score;
    }
}

//------------------------------------------------------------------------------
void
game::updateGhostDotCounters() {

    // Blinky's dot counter is never updated since it's
    // limit is always 0, the other ghost's dot counters
    // are updated for the first ghost in the house in
    // the following order:
    static ActorType ghosts[] = { Pinky, Inky, Clyde };
    for (ActorType type : ghosts) {
        if (House == state.actors[type].state) {
            state.actors[type].dotCounter++;
            break;
        }
    }
}

//------------------------------------------------------------------------------
void
game::eatDot(canvas* canvas, sound* sound) {
    const Int2& pacPos = state.actors[Pacman].tilePos;
    this->setTile(pacPos, Empty);
    canvas->SetTile(Sheet::Space, pacPos.x, pacPos.y);
    this->updateGhostDotCounters();
    this->addScore(1);
    state.noDotFrames = 0;
    state.dotCounter++;
    if (state.dotCounter & 1) {
        sound->Ka();
    }
    else {
        sound->Wa();
    }
}

//------------------------------------------------------------------------------
void
game::eatEnergizer(Energizer& energizer, sound* sound) {
    energizer.active = false;
    state.ghostKillCounter = 0;
    for (Actor& actor : state.actors) {
        if (Pacman != actor.type) {
            // FIXME: 6 seconds is only for the first level
            actor.frightenedTick = 6 * 60;
        }
    }
    this->addScore(5);
}

//------------------------------------------------------------------------------
void
game::killPacman() {
    Log::Info("PACMAN KILLED!\n");
    Actor& pacman = state.actors[Pacman];
    pacman.killedCount++;
    pacman.killedTicks = PacmanDeathTicks;
    state.blockCounter = PacmanDeathTicks;
}

//------------------------------------------------------------------------------
void
game::killGhost(Actor& ghost, sound* sound) {
    ghost.state = Hollow;
    ghost.frightenedTick = 0;
    ghost.killedCount = ++state.ghostKillCounter;
    ghost.killedTicks = GhostDeathTicks;
    state.blockCounter = GhostDeathTicks;
    int score = 0;
    switch (ghost.killedCount) {
        case 1: score = 20; break;
        case 2: score = 40; break;
        case 3: score = 80; break;
        default: score = 160; break;
    }
    this->addScore(score);
    sound->EatGhost();
}

} // namespace Paclone
