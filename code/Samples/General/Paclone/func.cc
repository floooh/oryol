//------------------------------------------------------------------------------
//  func.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "func.h"
#include "Core/Assertion.h"
#include <cstdlib>

namespace Paclone {
    
//------------------------------------------------------------------------------
int
func::clamp(int i, int lim) {
    if (i < 0) return 0;
    else if (i >= lim) return lim-1;
    else return i;
}
    
//------------------------------------------------------------------------------
Int2
func::clamp(Int2 i, Int2 lim) {
    return Int2{clamp(i.x, lim.x), clamp(i.y, lim.y)};
}
    
//------------------------------------------------------------------------------
bool
func::nearEqual(int i0, int i1, int maxDiff) {
    int diff = i1 - i0;
    diff = diff >= 0 ? diff : -diff;
    o_assert(diff >= 0);
    return diff <= maxDiff;
}
    
//------------------------------------------------------------------------------
bool
func::equal(const Int2& v0, const Int2& v1) {
    return (v0.x == v1.x) && (v0.y == v1.y);
}

//------------------------------------------------------------------------------
TileType
func::tileType(Int2 pos) {
    Int2 clampedPos = clamp(pos, Int2{Width, Height});
    return state::tileMap[clampedPos.y][clampedPos.x];
};
    
//------------------------------------------------------------------------------
Int2
func::homePixelPos(Int2 tilePos) {
    Int2 res;
    res.x = tilePos.x * TileSize + TileMidX + TileSize/2;
    res.y = tilePos.y * TileSize + TileMidY;
    return res;
}
    
//------------------------------------------------------------------------------
Int2
func::pixelDrawPos(Int2 pixelPos) {
    Int2 res;
    res.x = pixelPos.x - TileMidX - TileSize/2;
    res.y = pixelPos.y - TileMidY - TileSize/2;
    return res;
}
    
//------------------------------------------------------------------------------
Int2
func::tileToPixelPos(Int2 tilePos) {
    Int2 res;
    res.x = tilePos.x * TileSize - TileSize/2;
    res.y = tilePos.y * TileSize - TileSize/2;
    return res;
}
    
//------------------------------------------------------------------------------
int
func::targetDistSq(Int2 pos, Int2 targetPos) {
    int dx = pos.x - targetPos.x;
    int dy = pos.y - targetPos.y;
    return dx * dx + dy * dy;
}
    
//------------------------------------------------------------------------------
GhostState
func::lookupScatterChaseMode(int tick) {
    for (int i = 0; i < NumScatterChasePhases; i++) {
        if (tick < state::scatterChaseTable[i].frame) {
            return (GhostState) state::scatterChaseTable[i].state;
        }
    }
    // can't happen
    return Chase;
}
    
//------------------------------------------------------------------------------
bool
func::isBlocked(GhostState state, Int2 tilePos) {
    TileType tile = func::tileType(tilePos);
    if ((Hollow == state) || (LeaveHouse == state)) {
        return (Wall == tile);
    }
    else {
        return (Wall == tile) || (Door == tile);
    }
}

//------------------------------------------------------------------------------
int
func::actorSpeed(const Actor& actor, int gameTick) {
    // determine the speed of an actor by returning whether a
    // frame should be skipped
    if (actor.type == Pacman) {
        // Pacman normally moves at 80% (skipping every 5th frame)
        return (0 != gameTick % 5) ? 1 : 0;
    }
    else if (actor.state == Frightened) {
        // frightened: move at half speed
        return gameTick & 1;
    }
    else if (actor.state == Hollow) {
        // hollow: ~1.5x movement speed
        return (gameTick & 1) ? 1 : 2;
    }
    else {
        // normal ghost speed is 75% (skip every 4th frame)
        // ghost speed in tunnel is 40%
        if ((actor.tilePos.y == 17) && ((actor.tilePos.x <= 5) || (actor.tilePos.x >= 22))) {
            return (0 != ((gameTick * 2) % 4));
        }
        else {
            return (0 != gameTick % 4) ? 1 : 0;
        }
    }
}
    
//------------------------------------------------------------------------------
bool
func::canMove(const Actor& actor, Direction dir, bool allowCornering) {
    const Int2& dirVec = state::dirVec[dir];
    
    // get distance to midpoint in move direction and other direction
    int perpDistToMid, moveDistToMid;
    if (dirVec.y != 0) {
        perpDistToMid = actor.distToMid.x;
        moveDistToMid = actor.distToMid.y;
    }
    else {
        perpDistToMid = actor.distToMid.y;
        moveDistToMid = actor.distToMid.x;
    }
    
    // look one tile ahead in movement direction
    Int2 checkPos{actor.tilePos.x + dirVec.x, actor.tilePos.y + dirVec.y};
    bool isBlocked = func::isBlocked(actor.state, checkPos);
    if ((!allowCornering && (0 != perpDistToMid)) || (isBlocked && (0 == moveDistToMid))) {
        // way is blocked
        return false;
    }
    else {
        // way is free
        return true;
    }
}

//------------------------------------------------------------------------------
Direction
func::checkDir(const Actor& actor, Direction wantedDir, bool allowCornering) {
    
    // Movement rules for Pacman and Ghosts are different: see 'Cornering':
    // http://home.comcast.net/~jpittman2/pacman/pacmandossier.html#CH2_Cornering

    // dir is the direction the actor *wants* to move which might not be possible
    if ((NoDirection != wantedDir) && func::canMove(actor, wantedDir, allowCornering)) {
        // can move in intended direction
        return wantedDir;
    }
    else {
        // movement into intended direction is not possible, check
        // whether movement in current direction is possible
        if (!func::canMove(actor, actor.dir, allowCornering)) {
            // nope, emergency stop
            return NoDirection;
        }
        else {
            // keep moving into current direction
            return actor.dir;
        }
    }
}
    
//------------------------------------------------------------------------------
Int2
func::chooseScatterTarget(const Actor& ghost) {
    return state::ghostScatterTargets[ghost.type];
}

//------------------------------------------------------------------------------
Int2
func::chooseChaseTarget(const Actor& ghost, const Actor& pacman, const Actor& blinky) {
    Int2 target;
    if (Blinky == ghost.type) {
        // directly target pacman
        target = pacman.tilePos;
    }
    else if (Pinky == ghost.type) {
        // target 4 tiles ahead of pacman
        // this does not reproduce the 'diagonal up' overflow
        target.x =  pacman.tilePos.x + 4 * state::dirVec[pacman.dir].x;
        target.y = pacman.tilePos.y + 4 * state::dirVec[pacman.dir].y;
    }
    else if (Inky == ghost.type) {
        const int mx = pacman.tilePos.x + 2 * state::dirVec[pacman.dir].x;
        const int my = pacman.tilePos.y + 2 * state::dirVec[pacman.dir].y;
        const int dx = mx - blinky.tilePos.x;
        const int dy = my - blinky.tilePos.y;
        target.x = blinky.tilePos.x + 2 * dx;
        target.y = blinky.tilePos.y + 2 * dy;
    }
    else if (Clyde == ghost.type) {
        const int dx = ghost.tilePos.x - pacman.tilePos.x;
        const int dy = ghost.tilePos.y - pacman.tilePos.y;
        if ((dx*dx + dy*dy) > 64) {
            target = pacman.tilePos;
        }
        else {
            target = Int2{0, 34};
        }
    }
    else {
        // can't happen
        o_assert_dbg(false);
    }
    return target;
}

//------------------------------------------------------------------------------
Int2
func::chooseFrightenedTarget(const Actor& ghost, int gameTick) {
    // simply choose a random target point now and then
    // FIXME: this should be a table lookup
    if (gameTick % 13) {
        return Int2{std::rand() % Width, std::rand() % Height};
    }
    else {
        return ghost.targetPos;
    }
}

//------------------------------------------------------------------------------
Int2
func::chooseHollowTarget(Actor& ghost) {
    // the monster house door position
    return Int2{13, 15};
}

} // namespace Paclone