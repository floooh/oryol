#pragma once
//------------------------------------------------------------------------------
/**
    Implement the pacman game loop.
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
    struct Actor {
        Sheet::SpriteId spriteId = Sheet::InvalidSprite;
        Direction dir = NoDirection;
        Oryol::int16 x = 0;     // tile coord
        Oryol::int16 y = 0;     // tile coord
        Oryol::int16 subX = 0;  // inside tile coord [0..7]
        Oryol::int16 subY = 0;  // inside tile coord [0..7]
        Oryol::int16 dx = 0; // movement x
        Oryol::int16 dy = 0; // movement y
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

    TileType tileMap[Height][Width]{ {Empty} };
};
    
} // namespace Paclone
