#pragma once
//------------------------------------------------------------------------------
/**
    @class Paclone::state
    
    Wraps the entire Paclone game state.
*/
#include "sprites.h"

namespace Paclone {

enum TileType {
    Empty = 0,
    Dot,
    Wall,
    Door,
    
    NumTileTypes,
};

enum ActorType {
    // don't change order!
    Blinky = 0,
    Pinky,
    Inky,
    Clyde,
    Pacman,
    
    NumActorTypes,
    InvalidActorType,
};

enum Direction {
    NoDirection = 0,
    Left,
    Right,
    Up,
    Down,
    
    NumDirections,
};

enum GhostState {
    None,
    Chase,
    Scatter,
    Frightened,
    Hollow,
    House,
    LeaveHouse,
    EnterHouse,
};

struct Int2 {
    Int2() : x(0), y(0) {};
    Int2(int x_, int y_) : x(x_), y(y_) {};
    int x;
    int y;
};

struct Actor {
    ActorType type = InvalidActorType;
    int spriteIndex = 0;
    Direction dir = NoDirection;
    Direction nextDir = NoDirection;
    int moveTick = 0;
    Int2 pixelPos;
    Int2 tilePos;
    Int2 distToMid;
    Int2 targetPos;
    Int2 homeBasePixelPos;
    GhostState state = Scatter;
    int frightenedTick = 0;
    int dotCounter = 0;
    int dotLimit = 0;
    int killedCount = 0;    // 1, 2, 3 or 4th killed ghost
    int killedTicks = 0;    // count down ticks after killed
    bool forceLeaveHouse = false;
};

struct Energizer {
    Int2 tilePos;
    int spriteIndex = 0;
    bool active = false;
};

static const int NumLives = 3;
static const int Width = 28;
static const int Height = 36;
static const int NumEnergizers = 4;
static const int NumGhosts = 4;
static const int NumSprites = NumEnergizers + NumActorTypes + NumLives;
static const int TileSize = 8;
static const int TileMidX = 3;
static const int TileMidY = 4;
static const int AntePortasPixelX = 13 * TileSize + TileMidX + TileSize/2;
static const int AntePortasPixelY = 14 * TileSize + TileMidY;
static const int NumScatterChasePhases = 8;
static const int PacmanDeathTicks = 180;
static const int GhostDeathTicks = 30;

class state {
public:

    int gameTick = 0;
    int roundCounter = 0;   // each new life is a 'round'
    int blockCounter = 0;   // game 'freezes' as long as blockCounter > 0
    int dotCounter = 0;     // number of dots eaten by pacman
    int noDotFrames = 0;    // number of frames since last dot eaten
    int score = 0;
    int hiscore = 0;
    int lives = NumLives;
    int ghostKillCounter = 0;   // consecutive ghost kill counter
    Actor actors[NumActorTypes];
    Energizer energizers[NumEnergizers];

    static const char* charMap;
    static TileType tileMap[Height][Width];
    static const Sheet::SpriteId defaultSpriteMap[NumActorTypes][NumDirections];
    static const Sheet::SpriteId hollowSpriteMap[NumDirections];
    static const Sheet::SpriteId killedGhostMap[NumGhosts];
    static const Direction reverseDir[NumDirections];
    static const Int2 dirVec[NumDirections];
    static const Int2 homeTilePos[NumActorTypes];
    static const struct scatterChase {
        int frame;
        GhostState state;
    } scatterChaseTable[NumScatterChasePhases];
    static const Int2 energizerPos[NumEnergizers];
    static const Direction actorStartDir[NumActorTypes];
    static const int actorDotLimits[NumActorTypes];
    static const Int2 ghostScatterTargets[NumActorTypes];
};
    
} // namespace Paclone
    