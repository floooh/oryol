#pragma once
//------------------------------------------------------------------------------
#include "Core/Containers/StaticArray.h"
#include "Core/Assertion.h"
#include "Input/InputTypes.h"
#include "Input/private/gestureDetectors.h"
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include <bitset>

namespace Oryol {
namespace _priv {

class inputDispatcher;

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::keyboardDevice
    @ingroup _priv
    @brief keyboard state
*/
class keyboardDevice {
public:
    keyboardDevice();
    bool keyPressed(Key::Code key) const;
    bool keyDown(Key::Code key) const;
    bool keyUp(Key::Code key) const;
    bool keyRepeat(Key::Code key) const;
    bool anyKeyPressed() const;
    bool anyKeyDown() const;
    bool anyKeyUp() const;
    bool anyKeyRepeat() const;
    const wchar_t* capturedText() const;
    void onKeyDown(Key::Code key);
    void onKeyUp(Key::Code key);
    void onKeyRepeat(Key::Code key);
    void onChar(wchar_t c);
    void reset();
    void clearCapturedText();

    bool attached = false;
    inputDispatcher* dispatcher = nullptr;
    std::bitset<Key::NumKeys> down;
    std::bitset<Key::NumKeys> up;
    std::bitset<Key::NumKeys> pressed;
    std::bitset<Key::NumKeys> repeat;
    static const int MaxNumChars = 128;
    int charIndex = 0;
    StaticArray<wchar_t, MaxNumChars+1> chars;
};


//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mouseDevice
    @ingroup _priv
    @brief mouse state
*/
class mouseDevice {
public:
    mouseDevice();
    bool buttonPressed(MouseButton::Code btn) const;
    bool buttonDown(MouseButton::Code btn) const;
    bool buttonUp(MouseButton::Code btn) const;
    PointerLockMode::Code onButtonDown(MouseButton::Code btn);
    PointerLockMode::Code onButtonUp(MouseButton::Code btn);
    void onPosMov(const glm::vec2& pos);
    void onPosMov(const glm::vec2& pos, const glm::vec2& mov);
    void onMov(const glm::vec2& mov);
    void onScroll(const glm::vec2& scroll);
    void reset();

    bool attached = false;
    glm::vec2 position{0.0f, 0.0f};
    glm::vec2 movement{0.0f, 0.0f};
    glm::vec2 scroll{0.0f, 0.0f};
    inputDispatcher* dispatcher = nullptr;
    enum flags {
        btnDown = (1<<0),
        btnUp = (1<<1),
        btnPressed = (1<<2)
    };
    StaticArray<uint8_t, MouseButton::NumMouseButtons> buttonState;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::touchpadDevice
    @ingroup _priv
    @brief access to touchpad state
*/
class touchpadDevice {
public:
    void onTouchEvent(touchEvent::touchType type, const glm::vec2& pos);
    void onTapped(const glm::vec2& pos);
    void onDoubleTapped(const glm::vec2& pos);
    void onPanningStarted(const glm::vec2& pos, const glm::vec2& startPos);
    void onPanning(const glm::vec2& pos, const glm::vec2& startPos);
    void onPanningEnded(const glm::vec2& pos, const glm::vec2& startPos);
    void onPanningCancelled();
    void onPinchingStarted(const glm::vec2& pos0, const glm::vec2& pos1, const glm::vec2& startPos0, const glm::vec2& startPos1);
    void onPinching(const glm::vec2& pos0, const glm::vec2& pos1, const glm::vec2& startPos0, const glm::vec2& startPos1);
    void onPinchingEnded(const glm::vec2& pos0, const glm::vec2& pos1, const glm::vec2& startPos0, const glm::vec2& startPos1);
    void onPinchingCancelled();
    void reset();

    bool attached = false;
    static const int MaxNumTouches = 2;
    StaticArray<glm::vec2, MaxNumTouches> position;
    StaticArray<glm::vec2, MaxNumTouches> movement;
    StaticArray<glm::vec2, MaxNumTouches> startPosition;
    bool touchStarted = false;
    bool touchMoved = false;
    bool touchEnded = false;
    bool touchCancelled = false;
    bool tapped = false;
    bool doubleTapped = false;
    bool panningStarted = false;
    bool panning = false;
    bool panningEnded = false;
    bool pinchingStarted = false;
    bool pinching = false;
    bool pinchingEnded = false;
    inputDispatcher* dispatcher = nullptr;    
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::gamepadDevice
    @ingroup _priv
    @brief access to gamepad state
*/
class gamepadDevice {
public:
    bool buttonPressed(int rawBtnIndex) const;
    bool buttonDown(int rawBtnIndex) const;
    bool buttonUp(int rawBtnIndex) const;
    float axisValue(int rawAxisIndex) const;
    void reset();

    bool attached = false;
    StringAtom id;
    uint32_t down = 0;
    uint32_t up = 0;
    uint32_t pressed = 0;
    StaticArray<float, inputDefs::maxNumRawAxes> axes;
    GamepadMapping mapping;
};

//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::Sensors
    @ingroup Input
    @brief returns device sensory data
*/
class sensorDevice {
public:
    bool attached = false;
    glm::vec3 acceleration{0.0f, -9.80665f, 0.0f};
    glm::vec3 yawPitchRoll;
};

} // namespace _priv
} // namespace Oryol
