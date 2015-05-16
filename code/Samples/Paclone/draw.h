#pragma once
//------------------------------------------------------------------------------
/**
    @class Paclone::draw
    @brief map drawing functions
*/
#include "state.h"
#include "canvas.h"

namespace Paclone {
    
class draw {
public:
    /// setup the background tile map in the sprite canvas
    static void initMap(const state& state, canvas* canvas);
    /// draw the energizer sprites
    static void drawEnergizers(const state& state, canvas* canvas);
    /// draw actors
    static void drawActors(const state& state, canvas* canvas);
    /// draw playfield chrome (hiscore, lifes, ...)
    static void drawChrome(const state& state, canvas* canvas);
};
    
} // namespace Paclone