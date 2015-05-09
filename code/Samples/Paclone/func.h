#pragma once
//------------------------------------------------------------------------------
/**
    @class Paclone::func
    @brief pure functions which don't change any state
*/
#include "state.h"

namespace Paclone {

class func {
public:
    //--- clamp an int between (0, lim)
    static int clamp(int i, int lim);
    //--- clamp an Int2 between (0, lim)
    static Int2 clamp(Int2 i, Int2 lim);
    //--- check if diff of 2 ints is within maxDiff
    static bool nearEqual(int i0, int i1, int maxDiff);
    //--- check if 2 Int2's are equal
    static bool equal(const Int2& v0, const Int2& v1);
    //--- get tile type at map position
    static TileType tileType(Int2 pos);
    //--- return ghost home pixel position for a tile
    static Int2 homePixelPos(Int2 tilePos);
    //--- return sprite-drawing position for a tile poxel position
    static Int2 pixelDrawPos(Int2 pixelPos);
    //--- convert tile position to pixel position
    static Int2 tileToPixelPos(Int2 tilePos);
    //--- return squared distance to target position
    static int targetDistSq(Int2 pos, Int2 targetPos);
    //--- return whether ghost is in Scatter or Chase mode by game tick
    static GhostState lookupScatterChaseMode(int tick);
    //--- check if a tile position is blocking for an ghost state
    static bool isBlocked(GhostState state, Int2 tilePos);
    // compute actor speed (number of moves this frame)
    static int actorSpeed(const Actor& actor, int gameTick);
    //--- test if movement in a given direction is possible for actor
    static bool canMove(const Actor& actor, Direction dir, bool allowCornering);
    //--- filter intended movement direction, return valid direction (or NoDirection)
    //--- FIXME: this function is kind of broken, see where it is called!
    static Direction checkDir(const Actor& actor, Direction wantedDir, bool allowCornering);
    //--- choose ghost target in scatter mode
    static Int2 chooseScatterTarget(const Actor& ghost);
    //--- choose ghost target in chase mode
    static Int2 chooseChaseTarget(const Actor& ghost, const Actor& pacman, const Actor& blinky);
    //--- choose ghost target position in frightened mode
    static Int2 chooseFrightenedTarget(const Actor& ghost, int gameTick);
    //--- choose ghost target position in hollow mode
    static Int2 chooseHollowTarget(Actor& ghost);
};
    
} // namespace Paclone