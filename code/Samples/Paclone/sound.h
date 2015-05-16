#pragma once
//------------------------------------------------------------------------------
/**
    Pacman sound effects.
    
    http://www.lomont.org/Software/Games/PacMan/PacmanEmulation.pdf
*/
#include "Core/Types.h"
#include "Resource/Id.h"

namespace Paclone {

class sound {
public:
    /// setup sound effects
    void CreateSoundEffects();
    /// reset internal tick counters, call at round start
    void Reset();

    /// length of intro song in number of ticks
    static const int IntroSongLengthTicks = int(4.28f * 60);
    /// ghost frightened sound number of ticks
    static const int GhostFrightenedLengthTicks = int(0.1333f * 60);
    /// ghost normal loop number of ticks
    static const int GhostNormalLengthTicks = int(0.395f * 60);
    /// ghost alarm loop length in ticks
    static const int GhostAlarmLengthTicks = int(0.245f * 60);

    /// play game session intro song
    void IntroSong();
    /// play first part of waka sound effect (even dots)
    void Wa();
    /// play second part of waka sound effect (odd dots)
    void Ka();
    /// play the ghost-frightened sound
    void GhostFrightened(int tick);
    /// normal ghost loop
    void GhostNormal(int tick);
    /// ghost alarm loop
    void GhostAlarm(int tick);
    /// ghost alarm (dead and returning to monster den)
    /// play sound when eating a ghost
    void EatGhost();

private:
    Oryol::Id introSong;
    Oryol::Id wa;
    Oryol::Id ka;
    Oryol::Id ghostFrightened;
    Oryol::Id ghostNormal;
    Oryol::Id ghostAlarm;
    Oryol::Id eatGhost;
    int ghostFrightenedTick = 0;
    int ghostNormalTick = -GhostNormalLengthTicks;
    int ghostAlarmTick = 0;
};

} // namespace Paclone