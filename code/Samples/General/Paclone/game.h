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
#include "state.h"
#include "func.h"

namespace Paclone {

class game {
public:
    /// constructor
    game();
    
    /// initiliaze (called once)
    void Init(canvas* canvas);
    /// update the game (called per frame)
    void Update(int tick, canvas* canvas, Direction input);
    /// cleanup (called once)
    void Cleanup();
    
private:
    void setTile(Int2 pos, TileType type) {
        Int2 clampedPos = func::clamp(pos, Int2{Width, Height});
        state::tileMap[clampedPos.y][clampedPos.x] = type;
    };
    static void commitPosition(Actor& actor) {
        actor.tilePos.x = actor.pixelPos.x / TileSize;
        actor.tilePos.y = actor.pixelPos.y / TileSize;
        actor.distToMid.x = TileMidX - actor.pixelPos.x % TileSize;
        actor.distToMid.y = TileMidY - actor.pixelPos.y % TileSize;
    };

    void setupTiles();
    void setupActors();
    void setupEnergizers();
    bool checkNewRound() const;
    void startNewRound();
    void updateActors(Direction input);
    void drawActors(canvas* canvas) const;
    void drawEnergizers(canvas* canvas) const;
    void drawChrome(canvas* canvas) const;
    void move(Actor& actor, bool allowCornering) const;
    void handleCollide(canvas* canvas);
    void eatDot(canvas* canvas);
    void eatEnergizer(Energizer& energizer);
    void killPacman();
    void killGhost(Actor& ghost);
    void updateCounters();
    void updateGhostDirection(Actor& ghost) const;
    void updateHouseDirection(Actor& ghost) const;
    void updateEnterHouseDirection(Actor& ghost) const;
    void updateLeaveHouseDirection(Actor& ghost) const;
    void updateGhostState(Actor& ghost);
    void updateGhostDotCounters();
    void addScore(int val);

    class state state;
};
    
} // namespace Paclone
