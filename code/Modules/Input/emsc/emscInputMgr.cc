//------------------------------------------------------------------------------
//  emscInputMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "emscInputMgr.h"
#include "Core/Core.h"
#include "Core/Time/Clock.h"
#include "Core/Log.h"
#include "glm/glm.hpp"

namespace Oryol {
namespace _priv {
    
//------------------------------------------------------------------------------
emscInputMgr::emscInputMgr() :
runLoopId(RunLoop::InvalidId),
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
    inputMgrBase::setup(setup);
    this->setupKeyTable();
    this->keyboard.attached = true;
    this->mouse.attached = true;
    this->touchpad.attached = true;
    this->sensors.attached = true;
    this->setupCallbacks();
    this->setupGamepads();
    this->runLoopId = Core::PostRunLoop()->Add([this]() { this->reset(); });
}

//------------------------------------------------------------------------------
void
emscInputMgr::discard() {
    this->discardCallbacks();
    Core::PostRunLoop()->Remove(this->runLoopId);
    this->runLoopId = RunLoop::InvalidId;
    inputMgrBase::discard();    
}

//------------------------------------------------------------------------------
void
emscInputMgr::setupCallbacks() {
    emscripten_set_keydown_callback(0, this, true, emscKeyDown);
    emscripten_set_keyup_callback(0, this, true, emscKeyUp);
    emscripten_set_keypress_callback(0, this, true, emscKeyPress);
    emscripten_set_mousedown_callback("#canvas", this, true, emscMouseDown);
    emscripten_set_mouseup_callback("#canvas", this, true, emscMouseUp);
    emscripten_set_mousemove_callback("#canvas", this, true, emscMouseMove);
    emscripten_set_wheel_callback("#canvas", this, false, emscWheel);
    emscripten_set_touchstart_callback("#canvas", this, true, emscTouch);
    emscripten_set_touchend_callback("#canvas", this, true, emscTouch);
    emscripten_set_touchmove_callback("#canvas", this, true, emscTouch);
    emscripten_set_touchcancel_callback("#canvas", this, true, emscTouch);
    emscripten_set_gamepadconnected_callback(this, true, emscGamepadConnected);
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
    emscripten_set_keydown_callback(0, 0, true, 0);
    emscripten_set_keyup_callback(0, 0, true, 0);    
    emscripten_set_keypress_callback(0, 0, true, 0);
    emscripten_set_mousedown_callback("#canvas", 0, true, 0);
    emscripten_set_mouseup_callback("#canvas", 0, true, 0);
    emscripten_set_mousemove_callback("#canvas", 0, true, 0);
    emscripten_set_wheel_callback("#canvas", 0, true, 0);
    emscripten_set_touchstart_callback("#canvas", 0, true, 0);
    emscripten_set_touchend_callback("#canvas", 0, true, 0);
    emscripten_set_touchmove_callback("#canvas", 0, true, 0);
    emscripten_set_touchcancel_callback("#canvas", 0, true, 0);
    emscripten_set_devicemotion_callback(0, true, 0);
    emscripten_set_deviceorientation_callback(0, true, 0);
    emscripten_set_gamepaddisconnected_callback(this, true, emscGamepadDisconnected);
}

/// gamepad
void
emscInputMgr::setupGamepads()
{
	int gamepadsAvailable = emscripten_get_num_gamepads();

	// TODO: Check if gamepadsAvailable <= MaxNumGamepads
	for (int i = 0; i < gamepadsAvailable; ++i)
	{
		this->gamepad[i].attached = true;
	}
}

//------------------------------------------------------------------------------
void
emscInputMgr::update()
{
	inputMgrBase::update();

    uint32_t down = 0;
    uint32_t up = 0;
    uint32_t pressed = 0;
    int gamepadsAvailable = emscripten_get_num_gamepads();
	EMSCRIPTEN_RESULT result = EMSCRIPTEN_RESULT_FAILED;
	EmscriptenGamepadEvent gamepadState;

	for (int i = 0; i < gamepadsAvailable; ++i)
	{
		result = emscripten_get_gamepad_status(i, &gamepadState);
		// TODO: check gamepadState validity
		if (EMSCRIPTEN_RESULT_SUCCESS != result)
		{
			continue;
		}

		down = 0;
		up = 0;
		pressed = 0;
		this->gamepad[i].down = 0;
		this->gamepad[i].up = 0;

		pressed  = (gamepadState.digitalButton[0] << GamepadGizmo::X);
		pressed |= (gamepadState.digitalButton[1] << GamepadGizmo::A);
		pressed |= (gamepadState.digitalButton[2] << GamepadGizmo::B);
		pressed |= (gamepadState.digitalButton[3] << GamepadGizmo::Y);
		pressed |= (gamepadState.digitalButton[4] << GamepadGizmo::LeftBumper);
		pressed |= (gamepadState.digitalButton[5] << GamepadGizmo::RightBumper);
		pressed |= (gamepadState.digitalButton[6] << GamepadGizmo::LeftTrigger);
		pressed |= (gamepadState.digitalButton[7] << GamepadGizmo::RightTrigger);
		pressed |= (gamepadState.digitalButton[8] << GamepadGizmo::Start);
		pressed |= (gamepadState.digitalButton[9] << GamepadGizmo::Back);
		pressed |= (gamepadState.digitalButton[10] << GamepadGizmo::LeftStick); // PS4 -> left stick click
		pressed |= (gamepadState.digitalButton[11] << GamepadGizmo::RightStick); // PS4 -> right stick click
//		pressed |= (gamepadState.digitalButton[12] << GamepadGizmo::DPadUp); // PS4 -> round PS button
//		pressed |= (gamepadState.digitalButton[13] << GamepadGizmo::DPadDown); // PS4 -> touch sensor click

		// If changed...
		GamepadGizmo::Code gizmoCode = GamepadGizmo::InvalidGamepadGizmo;
		for (int j = 0; j < 12; ++j)
		{
			gizmoCode = static_cast<GamepadGizmo::Code>(j);
			if (!((pressed & gizmoCode) && (this->gamepad[i].pressed & gizmoCode)))
			{
				// If currently pressed, must be down
				if (pressed & gizmoCode)
				{
					down &= gizmoCode;
				}
				else // must be up
				{
					up &= gizmoCode;
				}
			}
		}

		this->gamepad[i].down = down;
		this->gamepad[i].up = up;
		this->gamepad[i].pressed = pressed;

		// TODO: Where are the DPad values...?
//		this->gamepad[i].pressed |= (gamepadState.digitalButton[14] << GamepadGizmo::DPadUp);
//		this->gamepad[i].pressed |= (gamepadState.digitalButton[15] << GamepadGizmo::DPadDown);
//		this->gamepad[i].pressed |= (gamepadState.digitalButton[16] << GamepadGizmo::DPadLeft);
//		this->gamepad[i].pressed |= (gamepadState.digitalButton[17] << GamepadGizmo::DPadRight);

//		this->gamepad[i].values[GamepadGizmo::DPadUp].x = gamepadState.analogButton[0];
//		this->gamepad[i].pressed |= (gamepadState.analogButton[0] << GamepadGizmo::DPadDown);
//		this->gamepad[i].pressed |= (gamepadState.analogButton[14] << GamepadGizmo::DPadLeft);
//		this->gamepad[i].pressed |= (gamepadState.analogButton[17] << GamepadGizmo::DPadRight);

		this->gamepad[i].values[GamepadGizmo::LeftStick].x = gamepadState.axis[0];
		this->gamepad[i].values[GamepadGizmo::LeftStick].y = gamepadState.axis[1];
		this->gamepad[i].values[GamepadGizmo::RightStick].x = gamepadState.axis[2];
		this->gamepad[i].values[GamepadGizmo::LeftTrigger].x = gamepadState.axis[3]; // PS4 -> left trigger
		this->gamepad[i].values[GamepadGizmo::RightTrigger].x = gamepadState.axis[4]; // PS4 -> right trigger
		this->gamepad[i].values[GamepadGizmo::RightStick].y = gamepadState.axis[5];
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
        // returning false enabled keypress (WChar) events, but
        // also makes the browser react to Tab, Backspace, etc...
        // thus we need to filter those out
        switch (key) {
            case Key::Tab:
            case Key::BackSpace:
            case Key::Enter:
                return true;
            default:
                return false;
        }
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
        return true;
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
        emscripten_request_pointerlock(0, false);
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

/// gamepad
//typedef EM_BOOL (*em_gamepad_callback_func)(int eventType, const EmscriptenGamepadEvent *gamepadEvent, void *userData)
EM_BOOL
emscInputMgr::emscGamepadConnected(int eventType, const EmscriptenGamepadEvent* e, void* userData)
{
	emscInputMgr* self = (emscInputMgr*) userData;
	self->gamepad[e->index].attached = true;
	return true;
}

/// gamepad
//typedef EM_BOOL (*em_gamepad_callback_func)(int eventType, const EmscriptenGamepadEvent *gamepadEvent, void *userData)
EM_BOOL
emscInputMgr::emscGamepadDisconnected(int eventType, const EmscriptenGamepadEvent* e, void* userData)
{
	emscInputMgr* self = (emscInputMgr*) userData;
	self->gamepad[0].attached = false;
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
