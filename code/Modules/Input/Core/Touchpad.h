#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Touchpad
    @ingroup Input
    @brief access to touchpad state
*/
#include "Core/Types.h"
#include "glm/vec2.hpp"

namespace Oryol {
    
class Touchpad {
public:
    /// constructor
    Touchpad();
    
    /// touchpad valid?
    bool Attached;
    
    /// single-tapped?
    bool Tapped;
    /// double-tapped?
    bool DoubleTapped;
    /// pinching gesture underway?
    bool Pinching;
    /// panning gesture underway?
    bool Panning;

    /// current position (1 for tap/pan, 2 for pinch)
    const glm::vec2& Position(int32 touchIndex) const;
    /// current movement (1 for tap/pan, 2 for pinch)
    const glm::vec2& Movement(int32 touchIndex) const;

    /// update position
    void onPos(int32 touchIndex, const glm::vec2& p);
    /// update position and compute movement
    void onPosMov(int32 touchIndex, const glm::vec2& p);

    /// reset the touchpad state
    void reset();

private:
    static const int32 MaxNumTouches = 2;
    glm::vec2 pos[MaxNumTouches];
    glm::vec2 mov[MaxNumTouches];
};

//------------------------------------------------------------------------------
inline const glm::vec2&
Touchpad::Position(int32 touchIndex) const {
    o_assert_range_dbg(touchIndex, MaxNumTouches);
    return this->pos[touchIndex];
}

//------------------------------------------------------------------------------
inline const glm::vec2&
Touchpad::Movement(int32 touchIndex) const {
    o_assert_range_dbg(touchIndex, MaxNumTouches);
    return this->mov[touchIndex];
}

} // namespace Oryol
