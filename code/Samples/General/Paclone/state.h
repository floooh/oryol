#pragma once
//------------------------------------------------------------------------------
/**
    @class Paclone::state
    
    Wraps the entire Paclone game state.
*/
#include "data/sprites.h"

namespace Paclone {

enum TileType {
    Empty = 0,
    Dot,
    Wall,
    Door,
    
    NumTileTypes,
};

enum ActorType {
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
    int x;
    int y;
};

struct Actor {
    ActorType type = InvalidActorType;
    int spriteIndex = 0;
    Direction dir = NoDirection;
    Direction nextDir = NoDirection;
    int moveTick = 0;
    Int2 pixelPos{0,0};
    Int2 tilePos{0,0};
    Int2 distToMid{0,0};
    Int2 targetPos{0,0};
    Int2 homeBasePixelPos{0,0};
    GhostState state = Scatter;
    int frightenedTick = 0;
    int dotCounter = 0;
    int dotLimit = 0;
    bool forceLeaveHouse = false;
};

struct Energizer {
    Int2 tilePos{0,0};
    int spriteIndex = 0;
    bool active = false;
};

static const int NumLives = 3;
static const int Width = 28;
static const int Height = 36;
static const int NumEnergizers = 4;
static const int NumSprites = NumEnergizers + NumActorTypes + NumLives;
static const int TileSize = 8;
static const int TileMidX = 3;
static const int TileMidY = 4;
static const int AntePortasPixelX = 13 * TileSize + TileMidX + TileSize/2;
static const int AntePortasPixelY = 14 * TileSize + TileMidY;
static const int NumScatterChasePhases = 8;

class state {
public:

    int gameTick = 0;
    int dotCounter = 0;
    int noDotFrames = 0;    // number of frames since last dot eaten
    int score = 0;
    int hiscore = 0;
    int lives = NumLives;
    Actor actors[NumActorTypes];
    Energizer energizers[NumEnergizers];

    static const char* charMap;
    static TileType tileMap[Height][Width];
    static const Sheet::SpriteId defaultSpriteMap[NumActorTypes][NumDirections];
    static const Sheet::SpriteId hollowSpriteMap[NumDirections];
    static const Direction reverseDir[NumDirections];
    static const Int2 dirVec[NumDirections];
    static const Int2 homeTilePos[NumActorTypes];
    static const struct scatterChase {
        Oryol::int32 frame;
        GhostState state;
    } scatterChaseTable[NumScatterChasePhases];
    static const Int2 energizerPos[NumEnergizers];
    static const Direction actorStartDir[NumActorTypes];
    static const int actorDotLimits[NumActorTypes];
    static const Int2 ghostScatterTargets[NumActorTypes];
};
    
} // namespace Paclone
    