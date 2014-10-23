//------------------------------------------------------------------------------
//  state.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "state.h"
#include <limits>

namespace Paclone {

const char* state::charMap =
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
const Direction state::reverseDir[NumDirections] = {
    NoDirection, Right, Left, Down, Up,
};
    
// map direction enum to 2D movement vec
const Int2 state::dirVec[NumDirections] = {
    { 0, 0 }, { -1, 0 }, { +1, 0 }, { 0, -1 }, { 0, +1 }
};
    
// actor home positions/targets
const Int2 state::homeTilePos[NumActorTypes] = {
    { 13, 14 }, { 13, 17 }, { 11, 17 }, { 15, 17 }, { 13, 26 },
};
    
TileType state::tileMap[Height][Width] = { {Empty} };
    
// map actor types and movement direction to sprites
const Sheet::SpriteId state::defaultSpriteMap[NumActorTypes][NumDirections] = {
    { Sheet::BlinkyNoDir, Sheet::BlinkyLeft, Sheet::BlinkyRight, Sheet::BlinkyUp, Sheet::BlinkyDown },
    { Sheet::PinkyNoDir, Sheet::PinkyLeft, Sheet::PinkyRight, Sheet::PinkyUp, Sheet::PinkyDown },
    { Sheet::InkyNoDir, Sheet::InkyLeft, Sheet::InkyRight, Sheet::InkyUp, Sheet::InkyDown },
    { Sheet::ClydeNoDir, Sheet::ClydeLeft, Sheet::ClydeRight, Sheet::ClydeUp, Sheet::ClydeDown },
    { Sheet::PacmanNoDir, Sheet::PacmanLeft, Sheet::PacmanRight, Sheet::PacmanUp, Sheet::PacmanDown },
};
    
// map from direction to sprite for ghost hollow mode
const Sheet::SpriteId state::hollowSpriteMap[NumDirections] = {
    Sheet::EyesLeft, Sheet::EyesLeft, Sheet::EyesRight, Sheet::EyesUp, Sheet::EyesDown
};

// map to score sprites for 1st, 2nd, 3rd, 4th killed ghost
const Sheet::SpriteId state::killedGhostMap[NumGhosts] = {
    Sheet::P200, Sheet::P400, Sheet::P800, Sheet::P1600
};
    
// scatter/phase mode table (start tick, and phase)
const state::scatterChase state::scatterChaseTable[NumScatterChasePhases] = {
    { 7  * 60, Scatter },   // 7 seconds Scatter
    { 27 * 60, Chase },     // 20 seconds Chase
    { 34 * 60, Scatter },   // ...
    { 54 * 60, Chase },
    { 59 * 60, Scatter },
    { 79 * 60, Chase },
    { 84 * 60, Scatter },
    { std::numeric_limits<int>::max(), Chase },
};

// energizer positions
const Int2 state::energizerPos[NumEnergizers] = {
    { 1, 6 }, { 26, 6 }, { 1, 26 }, { 26, 26 },
};

// starting directions for actors
const Direction state::actorStartDir[NumActorTypes] = {
    Left, Down, Up, Up, NoDirection
};

// dot-eaten before leave-house limits per actor
const int state::actorDotLimits[NumActorTypes] = {
    0, 0, 30, 60, 0
};

// target positions when ghosts are in scatter mode
const Int2 state::ghostScatterTargets[NumActorTypes] = {
    { 25, 0 }, { 2, 0 }, {27, 34}, { 0, 34}, { 0, 0 },
};

} // namespace Paclone
