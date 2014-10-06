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
    /// started panning
    bool PanningStarted;
    /// panning gesture underway?
    bool Panning;
    /// ended panning
    bool PanningEnded;
    /// started pinching
    bool PinchingStarted;
    /// pinching underway
    bool Pinching;
    /// ended pinching
    bool PinchingEnded;
    
    /// current position (1 for tap/pan, 2 for pinch)
    const glm::vec2& Position(int32 touchIndex) const;
    /// current movement (1 for tap/pan, 2 for pinch)
    const glm::vec2& Movement(int32 touchIndex) const;
    /// start position (useful for tap/pan)
    const glm::vec2& StartPosition(int32 touchIndex) const;
    
    /// update position
    void onPos(int32 touchIndex, const glm::vec2& p);
    /// update position and compute movement
    void onPosMov(int32 touchIndex, const glm::vec2& p);
    /// update start position
    void onStartPos(int32 touchIndex, const glm::vec2& p);

    /// reset the touchpad state
    void reset();

private:
    static const int32 MaxNumTouches = 2;
    glm::vec2 pos[MaxNumTouches];
    glm::vec2 mov[MaxNumTouches];
    glm::vec2 startPos[MaxNumTouches];
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

//------------------------------------------------------------------------------
inline const glm::vec2&
Touchpad::StartPosition(int32 touchIndex) const {
    o_assert_range_dbg(touchIndex, MaxNumTouches);
    return this->startPos[touchIndex];
}

} // namespace Oryol
