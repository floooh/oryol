#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mouseDevice
    @ingroup _priv
    @brief access to mouse state
*/
#include "Core/Assertion.h"
#include "Core/Containers/StaticArray.h"
#include "Input/Core/InputEnums.h"
#include "glm/vec2.hpp"
#include <functional>

namespace Oryol {
namespace _priv {

class inputDispatcher;
    
class mouseDevice {
public:
    /// constructor
    mouseDevice();
    /// mouse attached?
    bool attached;
    /// test if mouse button is pressed
    bool buttonPressed(MouseButton::Code btn) const;
    /// test if mouse button was pressed down last frame
    bool buttonDown(MouseButton::Code btn) const;
    /// test if mouse button was release last frame
    bool buttonUp(MouseButton::Code btn) const;
    /// mouse position (unbounded pixel coords)
    glm::vec2 position;
    /// mouse movement since last frame
    glm::vec2 movement;
    /// scroll movement (usually provided by mouse wheel)
    glm::vec2 scroll;

    /// call when mouse button down event happens
    PointerLockMode::Code onButtonDown(MouseButton::Code btn);
    /// call when mouse button up event happens
    PointerLockMode::Code onButtonUp(MouseButton::Code btn);
    /// called to update position and computed movement
    void onPosMov(const glm::vec2& pos);
    /// update position and explicit movement 
    void onPosMov(const glm::vec2& pos, const glm::vec2& mov);
    /// called to update movement only
    void onMov(const glm::vec2& mov);
    /// called to update wheel scrolling
    void onScroll(const glm::vec2& scroll);
    /// reset the mouse state
    void reset();

    inputDispatcher* dispatcher = nullptr;
    enum flags {
        btnDown = (1<<0),
        btnUp = (1<<1),
        btnPressed = (1<<2)
    };
    StaticArray<uint8_t, MouseButton::NumMouseButtons> buttonState;
};

//------------------------------------------------------------------------------
inline bool
mouseDevice::buttonPressed(MouseButton::Code btn) const {
    o_assert_range_dbg(btn, MouseButton::NumMouseButtons);
    return 0 != (this->buttonState[btn] & btnPressed);
}

//------------------------------------------------------------------------------
inline bool
mouseDevice::buttonDown(MouseButton::Code btn) const {
    o_assert_range_dbg(btn, MouseButton::NumMouseButtons);
    return 0 != (this->buttonState[btn] & btnDown);
}

//------------------------------------------------------------------------------
inline bool
mouseDevice::buttonUp(MouseButton::Code btn) const {
    o_assert_range_dbg(btn, MouseButton::NumMouseButtons);
    return 0 != (this->buttonState[btn] & btnUp);
}

} // namespace _priv
} // namespace Oryol
