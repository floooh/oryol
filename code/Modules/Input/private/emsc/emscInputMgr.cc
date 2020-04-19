//------------------------------------------------------------------------------
//  emscInputMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "emscInputMgr.h"
#include "Core/Core.h"
#include "Core/Time/Clock.h"
#include "Core/Log.h"
#include "glm/glm.hpp"
#include <emscripten.h>

namespace Oryol {
namespace _priv {
    
//------------------------------------------------------------------------------
emscInputMgr::emscInputMgr() :
runLoopId(RunLoop::InvalidId),
updateGamepadsRunLoopId(RunLoop::InvalidId),
pointerLockActive(false) {
    // empty
}

//------------------------------------------------------------------------------
emscInputMgr::~emscInputMgr() {
    // empty
}

//------------------------------------------------------------------------------
void
emscInputMgr::setup(const InputSetup& setup) {
    this->setupGamepadMappings();   // needs to happen before calling base class
    inputMgrBase::setup(setup);
    this->setupKeyTable();
    this->keyboard.attached = true;
    this->mouse.attached = true;
    this->touchpad.attached = true;
    this->sensors.attached = true;
    this->setupCallbacks();
    this->updateGamepadsRunLoopId = Core::PreRunLoop()->Add([this]() { this->updateGamepads(); });
    this->runLoopId = Core::PostRunLoop()->Add([this]() { this->reset(); });
}

//------------------------------------------------------------------------------
void
emscInputMgr::discard() {
    this->discardCallbacks();
    Core::PreRunLoop()->Remove(this->updateGamepadsRunLoopId);
    Core::PostRunLoop()->Remove(this->runLoopId);
    this->runLoopId = RunLoop::InvalidId;
    inputMgrBase::discard();
}

//------------------------------------------------------------------------------
void
emscInputMgr::setupCallbacks() {
    o_assert(!this->inputSetup.HtmlElement.Empty());
    const char* canvasId = this->inputSetup.HtmlElement.AsCStr();
    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, true, emscKeyDown);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, true, emscKeyUp);
    emscripten_set_keypress_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, true, emscKeyPress);
    emscripten_set_mousedown_callback(canvasId, this, true, emscMouseDown);
    emscripten_set_mouseup_callback(canvasId, this, true, emscMouseUp);
    emscripten_set_mousemove_callback(canvasId, this, true, emscMouseMove);
    emscripten_set_wheel_callback(canvasId, this, false, emscWheel);
    emscripten_set_touchstart_callback(canvasId, this, true, emscTouch);
    emscripten_set_touchend_callback(canvasId, this, true, emscTouch);
    emscripten_set_touchmove_callback(canvasId, this, true, emscTouch);
    emscripten_set_touchcancel_callback(canvasId, this, true, emscTouch);
    if (this->inputSetup.AccelerometerEnabled) {
        emscripten_set_devicemotion_callback(this, true, emscDeviceMotion);
    }
    if (this->inputSetup.GyrometerEnabled) {
        emscripten_set_deviceorientation_callback(this, true, emscDeviceOrientation);
    }
}

//------------------------------------------------------------------------------
void
emscInputMgr::discardCallbacks() {
    o_assert(!this->inputSetup.HtmlElement.Empty());
    const char* canvasId = this->inputSetup.HtmlElement.AsCStr();
    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, true, 0);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, true, 0);    
    emscripten_set_keypress_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, 0, true, 0);
    emscripten_set_mousedown_callback(canvasId, 0, true, 0);
    emscripten_set_mouseup_callback(canvasId, 0, true, 0);
    emscripten_set_mousemove_callback(canvasId, 0, true, 0);
    emscripten_set_wheel_callback(canvasId, 0, true, 0);
    emscripten_set_touchstart_callback(canvasId, 0, true, 0);
    emscripten_set_touchend_callback(canvasId, 0, true, 0);
    emscripten_set_touchmove_callback(canvasId, 0, true, 0);
    emscripten_set_touchcancel_callback(canvasId, 0, true, 0);
    emscripten_set_devicemotion_callback(0, true, 0);
    emscripten_set_deviceorientation_callback(0, true, 0);
}

//------------------------------------------------------------------------------
void
emscInputMgr::setupGamepadMappings() {
    // need to identify OS we're running on :/
    int os = EM_ASM_INT_V(
        var ua = navigator.userAgent;
        if (ua.includes('OS X')) return 1;
        else if (ua.includes('Windows')) return 2;
        else return 3;
    );

    GamepadMapping m;
    if ((1 == os) || (2 == os)) {
        // OSX and Windows, on Windows, both the Xbox360 and PS4
        // gamepad behave identical, on OSX the PS4 gamepad
        // behaves identical, and the Xbox360 gamepad isn't 
        // recognized
        m.Buttons[GamepadButton::A] = 0;
        m.Buttons[GamepadButton::B] = 1;
        m.Buttons[GamepadButton::X] = 2;
        m.Buttons[GamepadButton::Y] = 3;
        m.Buttons[GamepadButton::LeftBumper] = 4;
        m.Buttons[GamepadButton::RightBumper] = 5;
        m.Buttons[GamepadButton::LeftTrigger] = 6;
        m.Buttons[GamepadButton::RightTrigger] = 7;
        m.Buttons[GamepadButton::Back] = 8;
        m.Buttons[GamepadButton::Start] = 9;
        m.Buttons[GamepadButton::LeftStick] = 10;
        m.Buttons[GamepadButton::RightStick] = 11;
        m.Buttons[GamepadButton::DPadUp] = 12;
        m.Buttons[GamepadButton::DPadDown] = 13;
        m.Buttons[GamepadButton::DPadLeft] = 14;
        m.Buttons[GamepadButton::DPadRight] = 15;
        m.Buttons[GamepadButton::Center] = 17;
        m.Axes[GamepadAxis::LeftStickHori].Axis = 0;
        m.Axes[GamepadAxis::LeftStickVert].Axis = 1;
        m.Axes[GamepadAxis::RightStickHori].Axis = 2;
        m.Axes[GamepadAxis::RightStickVert].Axis = 3;
        // FIXME: for the trigger axes, we would need to map 
        // emscripten gamepad event 'analogue buttons' to axes!
        this->defaultGamepadMapping = m;
    }
    else {
        // Linux
        // Xbox360 is default gamepad 
        // FIXME: triggers and dpad only exposed as axes
        m.Buttons[GamepadButton::A] = 0;
        m.Buttons[GamepadButton::B] = 1;
        m.Buttons[GamepadButton::X] = 2;
        m.Buttons[GamepadButton::Y] = 3;
        m.Buttons[GamepadButton::LeftBumper] = 4;
        m.Buttons[GamepadButton::RightBumper] = 5;
        m.Buttons[GamepadButton::Back] = 6;
        m.Buttons[GamepadButton::Start] = 7;
        m.Buttons[GamepadButton::LeftStick] = 9;
        m.Buttons[GamepadButton::RightStick] = 10;
        m.Buttons[GamepadButton::Center] = 8;
        m.Axes[GamepadAxis::LeftStickHori].Axis = 0;
        m.Axes[GamepadAxis::LeftStickVert].Axis = 1;
        m.Axes[GamepadAxis::RightStickHori].Axis = 3;
        m.Axes[GamepadAxis::RightStickVert].Axis = 4;
        m.Axes[GamepadAxis::LeftTrigger].Axis = 2;
        m.Axes[GamepadAxis::LeftTrigger].Scale = 0.5f;
        m.Axes[GamepadAxis::LeftTrigger].Bias = 0.5f;
        m.Axes[GamepadAxis::RightTrigger].Axis = 5;
        m.Axes[GamepadAxis::RightTrigger].Scale = 0.5f;
        m.Axes[GamepadAxis::RightTrigger].Bias = 0.5f;
        this->defaultGamepadMapping = m;

        // DS4 controller
        // FIXME: DPad only exposed as axes
        m = GamepadMapping();
        m.Buttons[GamepadButton::A] = 1;
        m.Buttons[GamepadButton::B] = 2;
        m.Buttons[GamepadButton::X] = 0;
        m.Buttons[GamepadButton::Y] = 3;
        m.Buttons[GamepadButton::LeftBumper] = 4;
        m.Buttons[GamepadButton::RightBumper] = 5;
        m.Buttons[GamepadButton::LeftTrigger] = 6;
        m.Buttons[GamepadButton::RightTrigger] = 7;
        m.Buttons[GamepadButton::Back] = 8;
        m.Buttons[GamepadButton::Start] = 9;
        m.Buttons[GamepadButton::LeftStick] = 10;
        m.Buttons[GamepadButton::RightStick] = 11;
        m.Axes[GamepadAxis::LeftStickHori].Axis = 0;
        m.Axes[GamepadAxis::LeftStickVert].Axis = 1;
        m.Axes[GamepadAxis::RightStickHori].Axis = 2;
        m.Axes[GamepadAxis::RightStickVert].Axis = 5;
        m.Axes[GamepadAxis::LeftTrigger].Axis = 3;
        m.Axes[GamepadAxis::LeftTrigger].Scale = 0.5f;
        m.Axes[GamepadAxis::LeftTrigger].Bias = 0.5f;
        m.Axes[GamepadAxis::RightTrigger].Axis = 4;
        m.Axes[GamepadAxis::RightTrigger].Scale = 0.5f;
        m.Axes[GamepadAxis::RightTrigger].Bias = 0.5f;
        this->addGamepadMapping("054c-05c4-Sony Computer Entertainment Wireless Controller", m);
    }
}

//------------------------------------------------------------------------------
void
emscInputMgr::updateGamepads() {
    if (emscripten_sample_gamepad_data() != EMSCRIPTEN_RESULT_SUCCESS) {
        // no gamepad API support in browser
        return;
    }
    for (int padIndex = 0; padIndex < inputDefs::maxNumGamepads; padIndex++) {
        EmscriptenGamepadEvent state = { };
        EMSCRIPTEN_RESULT result = emscripten_get_gamepad_status(padIndex, &state);
        // TODO: check gamepadState validity
        if (EMSCRIPTEN_RESULT_SUCCESS != result) {
            continue;
        }

        auto& pad = this->gamepad[padIndex];
        if (state.connected && !pad.attached) {
            pad.id = state.id;
            pad.mapping = this->lookupGamepadMapping(pad.id);
            pad.pressed = 0;
            pad.axes.Fill(0.0f);
        }
        else if (!state.connected && pad.attached) {
            pad.id.Clear();
            pad.pressed = 0;
            pad.axes.Fill(0.0f);
        }
        pad.attached = state.connected;
        if (pad.attached) {
            static_assert(inputDefs::maxNumRawButtons < 64, "gamepadDevice::MaxNumRawButtons");
            for (int rawBtnIndex = 0; rawBtnIndex < inputDefs::maxNumRawButtons; rawBtnIndex++) {
                uint32_t mask = (1<<rawBtnIndex);
                if (state.digitalButton[rawBtnIndex]) {
                    if ((pad.pressed & mask) == 0) {
                        pad.down |= mask;
                    }
                    pad.pressed |= mask;
                }
                else {
                    if ((pad.pressed & mask) != 0) {
                        pad.up |= mask;
                    }
                    pad.pressed &= ~mask;
                }
            }
            static_assert(inputDefs::maxNumRawAxes < 64, "gamepadDevice::MaxNumRawAxes");
            for (int rawAxisIndex = 0; rawAxisIndex < inputDefs::maxNumRawAxes; rawAxisIndex++) {
                pad.axes[rawAxisIndex] = state.axis[rawAxisIndex];
            }
        }
    }
}

//------------------------------------------------------------------------------
static bool
shouldConsumeKey(Key::Code key) {
    switch (key) {
        case Key::World1:
        case Key::World2:
        case Key::Escape:
        case Key::Enter:
        case Key::Tab:
        case Key::BackSpace:
        case Key::Insert:
        case Key::Delete:
        case Key::Right:
        case Key::Left:
        case Key::Down:
        case Key::Up:
        case Key::PageUp:
        case Key::PageDown:
        case Key::Home:
        case Key::End:
        case Key::CapsLock:
        case Key::ScrollLock:
        case Key::NumLock:
        case Key::PrintScreen:
        case Key::Pause:
        case Key::F1:
        case Key::F2:
        case Key::F3:
        case Key::F4:
        case Key::F5:
        case Key::F6:
        case Key::F7:
        case Key::F8:
        case Key::F9:
        case Key::F10:
        case Key::F11:
        case Key::F12:
        case Key::F13:
        case Key::F14:
        case Key::F15:
        case Key::F16:
        case Key::F17:
        case Key::F18:
        case Key::F19:
        case Key::F20:
        case Key::F21:
        case Key::F22:
        case Key::F23:
        case Key::F24:
        case Key::F25:
        case Key::NumEnter:
        case Key::LeftShift:
        case Key::LeftControl:
        case Key::LeftAlt:
        case Key::LeftSuper:
        case Key::RightShift:
        case Key::RightControl:
        case Key::RightAlt:
        case Key::RightSuper:
        case Key::Menu:
            return true;
        default:
            return false;
    }
}

//------------------------------------------------------------------------------
EM_BOOL
emscInputMgr::emscKeyDown(int eventType, const EmscriptenKeyboardEvent* e, void* userData) {
    emscInputMgr* self = (emscInputMgr*) userData;
    o_assert_dbg(self);
    const Key::Code key = self->mapKey(e->keyCode);
    if (Key::InvalidKey != key) {
        if (e->repeat) {
            self->keyboard.onKeyRepeat(key);
        }
        else {
            self->keyboard.onKeyDown(key);
        }
        // consume or forward the event ("printable" keys need to be forwarded)
        return shouldConsumeKey(key);
    }
    return false;
}

//------------------------------------------------------------------------------
EM_BOOL
emscInputMgr::emscKeyUp(int eventType, const EmscriptenKeyboardEvent* e, void* userData) {
    emscInputMgr* self = (emscInputMgr*) userData;
    o_assert_dbg(self);
    const Key::Code key = self->mapKey(e->keyCode);
    if (Key::InvalidKey != key) {
        self->keyboard.onKeyUp(key);
        // consume or forward the event ("printable" keys need to be forwarded)
        return shouldConsumeKey(key);
    }
    return false;
}

//------------------------------------------------------------------------------
EM_BOOL
emscInputMgr::emscKeyPress(int eventType, const EmscriptenKeyboardEvent* e, void* userData) {
    emscInputMgr* self = (emscInputMgr*) userData;
    o_assert_dbg(self);
    self->keyboard.onChar((wchar_t)e->charCode);    
    return true;
}

//------------------------------------------------------------------------------
MouseButton::Code
emscInputMgr::mapMouseButton(unsigned short html5Btn) const {
    switch (html5Btn) {
        case 0:  return MouseButton::Left;
        case 1:  return MouseButton::Middle;
        case 2:  return MouseButton::Right;
        default: return MouseButton::InvalidMouseButton;
    }
}

//------------------------------------------------------------------------------
bool
emscInputMgr::updatePointerLockMode(PointerLockMode::Code lockMode) {
    if (PointerLockMode::Enable == lockMode) {
        emscripten_request_pointerlock(EMSCRIPTEN_EVENT_TARGET_WINDOW, false);
        return true;
    }
    else {
        emscripten_exit_pointerlock();
        return false;
    }
}

//------------------------------------------------------------------------------
EM_BOOL
emscInputMgr::emscMouseDown(int eventType, const EmscriptenMouseEvent* e, void* userData) {
    emscInputMgr* self = (emscInputMgr*) userData;
    o_assert_dbg(self);
    const MouseButton::Code btn = self->mapMouseButton(e->button);
    if (MouseButton::InvalidMouseButton != btn) {
        PointerLockMode::Code lockMode = self->mouse.onButtonDown(btn);
        self->pointerLockActive = updatePointerLockMode(lockMode);
    }
    return true;
}

//------------------------------------------------------------------------------
EM_BOOL
emscInputMgr::emscMouseUp(int eventType, const EmscriptenMouseEvent* e, void* userData) {
    emscInputMgr* self = (emscInputMgr*) userData;
    o_assert_dbg(self);
    const MouseButton::Code btn = self->mapMouseButton(e->button);
    if (MouseButton::InvalidMouseButton != btn) {
        PointerLockMode::Code lockMode = self->mouse.onButtonUp(btn);
        self->pointerLockActive = updatePointerLockMode(lockMode);
    }
    return true;
}

//------------------------------------------------------------------------------
EM_BOOL
emscInputMgr::emscMouseMove(int eventType, const EmscriptenMouseEvent* e, void* userData) {
    emscInputMgr* self = (emscInputMgr*) userData;
    o_assert_dbg(self);
    // check if pointerlock is active, if yes directly obtain movement
    if (self->pointerLockActive) {
        const glm::vec2 mov((float)e->movementX, (float)e->movementY);
        self->mouse.onMov(mov);
    }
    else {
        const glm::vec2 pos((float)e->canvasX, (float)e->canvasY);
        self->mouse.onPosMov(pos);
    }
    return true;    
}

//------------------------------------------------------------------------------
EM_BOOL
emscInputMgr::emscWheel(int eventType, const EmscriptenWheelEvent* e, void* userData) {
    emscInputMgr* self = (emscInputMgr*) userData;
    o_assert_dbg(self);
    const glm::vec2 scroll((float)e->deltaX * 0.5f, -(float)e->deltaY * 0.5f);
    self->mouse.onScroll(scroll);
    return true;
}

//------------------------------------------------------------------------------
EM_BOOL
emscInputMgr::emscTouch(int eventType, const EmscriptenTouchEvent* e, void* userData) {
    emscInputMgr* self = (emscInputMgr*) userData;

    // convert to touchEvent
    touchEvent event;
    switch (eventType) {
        case EMSCRIPTEN_EVENT_TOUCHSTART:
            event.type = touchEvent::began;
            break;
        case EMSCRIPTEN_EVENT_TOUCHEND:
            event.type = touchEvent::ended;
            break;
        case EMSCRIPTEN_EVENT_TOUCHMOVE:
            event.type = touchEvent::moved;
            break;
        case EMSCRIPTEN_EVENT_TOUCHCANCEL:
            event.type = touchEvent::cancelled;
            break;
        default:
            return false;
    }
    event.time = Oryol::Clock::Now();
    event.numTouches = e->numTouches;
    for (int i = 0; i < event.numTouches; i++) {
        touchEvent::point& curPoint = event.points[i];
        curPoint.identifier = e->touches[i].identifier;
        curPoint.pos.x = e->touches[i].canvasX;
        curPoint.pos.y = e->touches[i].canvasY;
        curPoint.isChanged = e->touches[i].isChanged;
    }
    self->onTouchEvent(event);
    return true;
}

//------------------------------------------------------------------------------
EM_BOOL
emscInputMgr::emscDeviceMotion(int eventType, const EmscriptenDeviceMotionEvent* e, void* userData) {
    emscInputMgr* self = (emscInputMgr*) userData;

    self->sensors.acceleration.x = -e->accelerationIncludingGravityX;
    self->sensors.acceleration.y = -e->accelerationIncludingGravityY;
    self->sensors.acceleration.z = -e->accelerationIncludingGravityZ;

    return true;
}

//------------------------------------------------------------------------------
EM_BOOL
emscInputMgr::emscDeviceOrientation(int eventType, const EmscriptenDeviceOrientationEvent* e, void* userData) {
    emscInputMgr* self = (emscInputMgr*) userData;

    // FIXME: the roll angle needs some fixing
    self->sensors.yawPitchRoll.x = glm::radians(e->gamma);
    self->sensors.yawPitchRoll.y = glm::radians(e->beta);
    self->sensors.yawPitchRoll.z = glm::radians(e->alpha);

    return true;
}

//------------------------------------------------------------------------------
Key::Code
emscInputMgr::mapKey(unsigned long html5KeyCode) const {
    if (html5KeyCode < MaxNumKeys) {
        return this->keyTable[html5KeyCode];
    }
    else {
        return Key::InvalidKey;
    }
}

//------------------------------------------------------------------------------
void
emscInputMgr::setupKeyTable() {
    for (int i = 0; i < MaxNumKeys; i++) {
        this->keyTable[i] = Key::InvalidKey;
    }

    this->keyTable[8] = Key::BackSpace;
    this->keyTable[9] = Key::Tab;
    this->keyTable[13] = Key::Enter;
    this->keyTable[16] = Key::LeftShift;
    this->keyTable[17] = Key::LeftControl;
    this->keyTable[18] = Key::LeftAlt;
    this->keyTable[19] = Key::Pause;
    this->keyTable[27] = Key::Escape;
    this->keyTable[32] = Key::Space;
    this->keyTable[33] = Key::PageUp;
    this->keyTable[34] = Key::PageDown;
    this->keyTable[35] = Key::End;
    this->keyTable[36] = Key::Home;
    this->keyTable[37] = Key::Left;
    this->keyTable[38] = Key::Up;
    this->keyTable[39] = Key::Right;
    this->keyTable[40] = Key::Down;
    this->keyTable[45] = Key::Insert;
    this->keyTable[46] = Key::Delete;
    this->keyTable[48] = Key::N0;
    this->keyTable[49] = Key::N1;
    this->keyTable[50] = Key::N2;
    this->keyTable[51] = Key::N3;
    this->keyTable[52] = Key::N4;
    this->keyTable[53] = Key::N5;
    this->keyTable[54] = Key::N6;
    this->keyTable[55] = Key::N7;
    this->keyTable[56] = Key::N8;
    this->keyTable[57] = Key::N9;
    this->keyTable[59] = Key::Semicolon;
    this->keyTable[64] = Key::Equal;
    this->keyTable[65] = Key::A;
    this->keyTable[66] = Key::B;
    this->keyTable[67] = Key::C;
    this->keyTable[68] = Key::D;
    this->keyTable[69] = Key::E;
    this->keyTable[70] = Key::F;
    this->keyTable[71] = Key::G;
    this->keyTable[72] = Key::H;
    this->keyTable[73] = Key::I;
    this->keyTable[74] = Key::J;
    this->keyTable[75] = Key::K;
    this->keyTable[76] = Key::L;
    this->keyTable[77] = Key::M;
    this->keyTable[78] = Key::N;
    this->keyTable[79] = Key::O;
    this->keyTable[80] = Key::P;
    this->keyTable[81] = Key::Q;
    this->keyTable[82] = Key::R;
    this->keyTable[83] = Key::S;
    this->keyTable[84] = Key::T;
    this->keyTable[85] = Key::U;
    this->keyTable[86] = Key::V;
    this->keyTable[87] = Key::W;
    this->keyTable[88] = Key::X;
    this->keyTable[89] = Key::Y;
    this->keyTable[90] = Key::Z;
    this->keyTable[91] = Key::LeftSuper;
    this->keyTable[93] = Key::Menu;
    this->keyTable[96] = Key::Num0;
    this->keyTable[97] = Key::Num1;
    this->keyTable[98] = Key::Num2;
    this->keyTable[99] = Key::Num3;
    this->keyTable[100] = Key::Num4;
    this->keyTable[101] = Key::Num5;
    this->keyTable[102] = Key::Num6;
    this->keyTable[103] = Key::Num7;
    this->keyTable[104] = Key::Num8;
    this->keyTable[105] = Key::Num9;
    this->keyTable[106] = Key::NumMultiply;
    this->keyTable[107] = Key::NumAdd;
    this->keyTable[109] = Key::NumSubtract;
    this->keyTable[110] = Key::NumDecimal;
    this->keyTable[111] = Key::NumDivide;
    this->keyTable[112] = Key::F1;
    this->keyTable[113] = Key::F2;
    this->keyTable[114] = Key::F3;
    this->keyTable[115] = Key::F4;
    this->keyTable[116] = Key::F5;
    this->keyTable[117] = Key::F6;
    this->keyTable[118] = Key::F7;
    this->keyTable[119] = Key::F8;
    this->keyTable[120] = Key::F9;
    this->keyTable[121] = Key::F10;
    this->keyTable[122] = Key::F11;
    this->keyTable[123] = Key::F12;
    this->keyTable[144] = Key::NumLock;
    this->keyTable[145] = Key::ScrollLock;
    this->keyTable[173] = Key::Minus;
    this->keyTable[186] = Key::Semicolon;
    this->keyTable[187] = Key::Equal;
    this->keyTable[188] = Key::Comma;
    this->keyTable[189] = Key::Minus;
    this->keyTable[190] = Key::Period;
    this->keyTable[191] = Key::Slash;
    this->keyTable[192] = Key::GraveAccent;
    this->keyTable[219] = Key::LeftBracket;
    this->keyTable[220] = Key::BackSlash;
    this->keyTable[221] = Key::RightBracket;
    this->keyTable[222] = Key::Apostrophe;
    this->keyTable[224] = Key::LeftSuper;
}

} // namespace _priv
} // namespace Oryol
