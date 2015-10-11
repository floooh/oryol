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
#include "sound.h"
#include "func.h"
#include "sound.h"

namespace Paclone {

class game {
public:
    /// constructor
    game();
    
    /// initialize (called once)
    void Init(canvas* canvas, sound* sound);
    /// update the game (called per frame)
    void Update(int tick, canvas* canvas, sound* sound, Direction input);
    /// cleanup (called once)
    void Cleanup();
    /// get current pacman position
    const Int2& PacmanPos() const;
    
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

    void initTiles();
    void initActors();
    void initEnergizers();
    bool checkNewRound() const;
    void startNewRound(canvas* canvas, sound* sound);
    bool checkLifeLost() const;
    void startNewLife(sound* sound);
    void updateActors(Direction input);
    void move(Actor& actor, bool allowCornering) const;
    void handleCollide(canvas* canvas, sound* sound);
    void eatDot(canvas* canvas, sound* sound);
    void eatEnergizer(Energizer& energizer, sound* sound);
    void killPacman();
    void killGhost(Actor& ghost, sound* sound);
    void updateCounters();
    void updateGhostDirection(Actor& ghost) const;
    void updateHouseDirection(Actor& ghost) const;
    void updateEnterHouseDirection(Actor& ghost) const;
    void updateLeaveHouseDirection(Actor& ghost) const;
    void updateGhostState(Actor& ghost);
    void updateGhostDotCounters();
    void updateGhostSounds(sound* sound);
    void addScore(int val);

    class state state;
};
    
} // namespace Paclone
