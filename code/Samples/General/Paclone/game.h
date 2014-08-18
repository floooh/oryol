#pragma once
//------------------------------------------------------------------------------
/**
    Implement the pacman game loop.
    
    Kudos to:
    http://home.comcast.net/~jpittman2/pacman/pacmandossier.html
    And (especially for the cornering stuff!):
    http://pacman.shaunew.com/
*/
#include "Core/Types.h"
#include "canvas.h"

namespace Paclone {

class game {
public:
    /// static game elements
    enum TileType {
        Empty = 0,
        Pill,
        Wall,
        
        NumTileTypes,
    };
    /// dynamic game elements
    enum ActorType {
        Blinky = 0,
        Pinky,
        Inky,
        Clyde,
        Pacman,
        
        NumActorTypes,
    };
    /// directions
    enum Direction {
        NoDirection = 0,
        Left,
        Right,
        Up,
        Down,
        
        NumDirections,
    };
    /// ghost states
    enum GhostState {
        None,
        Chase,
        Scatter,
        Frightened,
        Hollow,
    };

    static const int Width = 28;
    static const int Height = 36;
    static const int NumEnergizers = 4;
    static const int NumSprites = NumEnergizers + NumActorTypes;
    
    /// constructor
    game();
    
    /// initiliaze (called once)
    void Init(canvas* canvas);
    /// update the game (called per frame)
    void Update(int tick, canvas* canvas, Direction input);
    /// cleanup (called once)
    void Cleanup();
    
private:
    static const int TileSize = 8;
    static const int TileMidX = 3;
    static const int TileMidY = 4;

    int gameTick = 0;
    
    struct Actor {
        ActorType type;
        int spriteIndex = 0;
        Direction dir = NoDirection;
        Direction nextDir = NoDirection;
        Oryol::int32 moveTick = 0;
        Oryol::int16 pixelX = 0;
        Oryol::int16 pixelY = 0;
        Oryol::int16 tileX  = 0;
        Oryol::int16 tileY  = 0;
        Oryol::int16 distToMidX = 0;
        Oryol::int16 distToMidY = 0;
        Oryol::int16 targetX = 0;
        Oryol::int16 targetY = 0;
        GhostState state = Scatter;
        int frightenedTick;
    } actors[NumActorTypes];
    
    struct Energizer {
        Oryol::int16 tileX = 0;
        Oryol::int16 tileY = 0;
        int spriteIndex = 0;
        bool active = false;
    } energizers[NumEnergizers];

    static Oryol::int16 clampX(Oryol::int16 x) {
        if (x < 0) return 0;
        else if (x >= Width) return Width - 1;
        else return x;
    };
    static Oryol::int16 clampY(Oryol::int16 y) {
        if (y < 0) return 0;
        else if (y >= Height) return Height - 1;
        else return y;
    };
    void setTile(Oryol::int16 x, Oryol::int16 y, TileType type) {
        this->tileMap[clampY(y)][clampX(x)] = type;
    };
    TileType tile(Oryol::int16 x, Oryol::int16 y) const {
        return this->tileMap[clampY(y)][clampX(x)];
    };
    static Oryol::int32 targetDistSq(Oryol::int16 x, Oryol::int16 y, Oryol::int16 targetX, Oryol::int16 targetY) {
        Oryol::int16 dx = x - targetX;
        Oryol::int16 dy = y - targetY;
        return dx * dx + dy * dy;
    };
    static void commitPosition(Actor& actor) {
        actor.tileX = actor.pixelX / TileSize;
        actor.tileY = actor.pixelY / TileSize;
        actor.distToMidX = TileMidX - actor.pixelX % TileSize;
        actor.distToMidY = TileMidY - actor.pixelY % TileSize;
    };

    void setupTiles();
    void setupActors();
    void setupEnergizers();
    void updateActors(Direction input);
    void drawActors(canvas* canvas) const;
    void drawEnergizers(canvas* canvas) const;
    bool canMove(const Actor& actor, Direction dir, bool allowCornering) const;
    void computeMove(Actor& actor, Direction dir) const;
    void move(Actor& actor) const;
    void handleCollide(canvas* canvas);
    void eatPill(canvas* canvas);
    void eatEnergizer(Energizer& energizer);
    void killPacman();
    void killGhost(Actor& ghost);
    void chooseScatterTarget(Actor& ghost) const;
    void chooseChaseTarget(Actor& ghost) const;
    void chooseFrightenedTarget(Actor& ghost) const;
    void chooseHollowTarget(Actor& ghost) const;
    void updateDirection(Actor& ghost) const;
    void updateGhostState(Actor& ghost);

    static TileType tileMap[Height][Width];
    static const Sheet::SpriteId defaultSpriteMap[NumActorTypes][NumDirections];
    static const Sheet::SpriteId hollowSpriteMap[NumDirections];
    static const Direction reverseDir[NumDirections];
    static const Oryol::int16 dirVec[NumDirections][2];
};
    
} // namespace Paclone
