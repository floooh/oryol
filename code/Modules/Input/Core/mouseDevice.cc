//------------------------------------------------------------------------------
//  mouseDevice.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mouseDevice.h"
#include "Input/Core/inputDispatcher.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mouseDevice::mouseDevice() :
attached(false),
position(0.0f, 0.0f),
movement(0.0f, 0.0f),
scroll(0.0f, 0.0f),
dispatcher(nullptr) {
    for (int i = 0; i < MouseButton::NumMouseButtons; i++) {
        this->buttonState[i] = 0;
    }
}

//------------------------------------------------------------------------------
PointerLockMode::Code
mouseDevice::onButtonDown(MouseButton::Code btn) {
    o_assert_dbg(this->dispatcher);

    this->buttonState[btn] |= (btnDown | btnPressed);
    InputEvent ie(InputEvent::MouseButtonDown, btn);
    this->dispatcher->notifyEvent(ie);
    return this->dispatcher->notifyPointerLock(ie);
}

//------------------------------------------------------------------------------
PointerLockMode::Code
mouseDevice::onButtonUp(MouseButton::Code btn) {
    o_assert_dbg(this->dispatcher);

    this->buttonState[btn] &= ~btnPressed;
    this->buttonState[btn] |= btnUp;
    InputEvent ie(InputEvent::MouseButtonUp, btn);
    this->dispatcher->notifyEvent(ie);
    return this->dispatcher->notifyPointerLock(ie);
}

//------------------------------------------------------------------------------
void
mouseDevice::onPosMov(const glm::vec2& pos) {
    o_assert_dbg(this->dispatcher);

    this->movement = pos - this->position;
    this->position = pos;
    this->dispatcher->notifyEvent(InputEvent(InputEvent::MouseMove, this->movement, this->position));
}

//------------------------------------------------------------------------------
void
mouseDevice::onPosMov(const glm::vec2& pos, const glm::vec2& mov) {
    o_assert_dbg(this->dispatcher);

    this->movement = mov;
    this->position = pos;
    this->dispatcher->notifyEvent(InputEvent(InputEvent::MouseMove, this->movement, this->position));
}

//------------------------------------------------------------------------------
void
mouseDevice::onMov(const glm::vec2& mov) {
    o_assert_dbg(this->dispatcher);

    this->movement = mov;
    this->dispatcher->notifyEvent(InputEvent(InputEvent::MouseMove, this->movement, this->position));
}

//------------------------------------------------------------------------------
void
mouseDevice::onScroll(const glm::vec2& scroll) {
    o_assert_dbg(this->dispatcher);

    this->scroll = scroll;
    this->dispatcher->notifyEvent(InputEvent(InputEvent::MouseScrolling, this->scroll));
}

//------------------------------------------------------------------------------
void
mouseDevice::reset() {
    for (int i = 0; i < MouseButton::NumMouseButtons; i++) {
        this->buttonState[i] &= ~(btnDown | btnUp);
    }
    this->movement = glm::vec2(0.0f, 0.0f);
    this->scroll = glm::vec2(0.0f, 0.0f);
}

} // namespace _priv
} // namespace Oryol
