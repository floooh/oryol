#pragma once
//------------------------------------------------------------------------------
/**
    Implement the pacman game loop.
    
    Cudos to:
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
        Energizer,
        Wall,
        
        NumTileTypes,
    };
    /// dynamic game elements
    enum ActorType {
        InvalidActorType = 0,
        Pacman,
        Ghost0,
        Ghost1,
        Ghost2,
        Ghost3,
        
        NumActorTypes,
    };
    /// directions
    enum Direction {
        NoDirection,
        Left,
        Right,
        Up,
        Down,
    };
    
    
    /// playfield size in 8x8 elements
    static const int Width = 28;
    static const int Height = 36;

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

    struct Actor {
        Sheet::SpriteId spriteId = Sheet::InvalidSprite;
        Direction dir = NoDirection;
        Oryol::int16 pixelX = 0;
        Oryol::int16 pixelY = 0;
        Oryol::int16 tileX  = 0;
        Oryol::int16 tileY  = 0;
        Oryol::int16 distToMidX = 0;
        Oryol::int16 distToMidY = 0;
        Oryol::int16 dirX = 0;
        Oryol::int16 dirY = 0;
    } actors[NumActorTypes];
    
    TileType tile(int x, int y) const;
    void setTile(int x, int y, TileType type);
    void setupTiles();
    void setupActors();
    void setupActor(ActorType type, int x, int y, Direction dir);
    void updateActors(Direction input);
    void updatePacman(Direction input);
    void drawActors(canvas* canvas);
    void drawActor(ActorType type, canvas* canvas);
    bool canMove(const Actor& actor, Direction dir);
    void computeMove(Actor& actor, Direction dir);
    void move(Actor& actor);
    void checkOverlaps(canvas* canvas);
    void eatPill(canvas* canvas);
    void eatEnergizer(canvas* canvas);
    void commitPosition(Actor& actor);

    TileType tileMap[Height][Width]{ {Empty} };
};
    
} // namespace Paclone
