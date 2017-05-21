//------------------------------------------------------------------------------
//  glfwInputMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glfwInputMgr.h"
#include "Gfx/private/glfw/glfwDisplayMgr.h"
#include "Core/Core.h"
#include "Core/RunLoop.h"
#include "GLFW/glfw3.h"

namespace Oryol {
namespace _priv {
    
glfwInputMgr* glfwInputMgr::self = nullptr;

static Key::Code keyTable[GLFW_KEY_LAST + 1];

//------------------------------------------------------------------------------
glfwInputMgr::glfwInputMgr() :
resetRunLoopId(RunLoop::InvalidId),
updateGamepadsRunLoopId(RunLoop::InvalidId) {
    o_assert(nullptr == self);
    self = this;
}

//------------------------------------------------------------------------------
glfwInputMgr::~glfwInputMgr() {
    o_assert(nullptr != self);
    self = nullptr;
}

//------------------------------------------------------------------------------
void
glfwInputMgr::setup(const InputSetup& setup) {
    
    this->setupGamepadMappings();   // must be called before parent class!
    inputMgrBase::setup(setup);
    this->keyboard.attached = true;
    this->mouse.attached = true;
    
    // first check that the Gfx module has already been initialized
    GLFWwindow* glfwWindow = _priv::glfwDisplayMgr::getGlfwWindow();
    if (nullptr == glfwWindow) {
        o_error("glfwInputMgr: Gfx::Setup must be called before Input::Setup!\n");
        return;
    }
    this->setupKeyTable();
    this->setupCallbacks(glfwWindow);

    // attach per-frame callbacks to the global runloop
    this->updateGamepadsRunLoopId = Core::PreRunLoop()->Add([this] { this->updateGamepads(); });
    this->resetRunLoopId = Core::PostRunLoop()->Add([this]() { this->reset(); });
}

//------------------------------------------------------------------------------
void
glfwInputMgr::discard() {
    
    // remove glfw input callbacks
    GLFWwindow* glfwWindow = _priv::glfwDisplayMgr::getGlfwWindow();
    o_assert(nullptr != glfwWindow);
    this->discardCallbacks(glfwWindow);
    
    // detach our reset callback from runloop
    Core::PostRunLoop()->Remove(this->resetRunLoopId);
    this->resetRunLoopId = RunLoop::InvalidId;
    Core::PreRunLoop()->Remove(this->updateGamepadsRunLoopId);
    this->updateGamepadsRunLoopId = RunLoop::InvalidId;
    
    inputMgrBase::discard();
}

//------------------------------------------------------------------------------
void
glfwInputMgr::setupCallbacks(GLFWwindow* glfwWindow) {
    glfwSetKeyCallback(glfwWindow, keyCallback);
    glfwSetCharCallback(glfwWindow, charCallback);
    glfwSetMouseButtonCallback(glfwWindow, mouseButtonCallback);
    glfwSetCursorPosCallback(glfwWindow, cursorPosCallback);
    glfwSetCursorEnterCallback(glfwWindow, cursorEnterCallback);
    glfwSetScrollCallback(glfwWindow, scrollCallback);
}

//------------------------------------------------------------------------------
void
glfwInputMgr::discardCallbacks(GLFWwindow* glfwWindow) {    
    glfwSetKeyCallback(glfwWindow, nullptr);
    glfwSetCharCallback(glfwWindow, nullptr);
    glfwSetMouseButtonCallback(glfwWindow, nullptr);
    glfwSetCursorPosCallback(glfwWindow, nullptr);
    glfwSetCursorEnterCallback(glfwWindow, nullptr);
    glfwSetScrollCallback(glfwWindow, nullptr);
}

//------------------------------------------------------------------------------
void
glfwInputMgr::setupGamepadMappings() {
    GamepadMapping m;
    #if ORYOL_OSX
    // PS4 dualshock, OSX
    // NOTE: the Xbox360 wired controller is not recognized on OSX
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
    m.Buttons[GamepadButton::DPadUp] = 14;
    m.Buttons[GamepadButton::DPadDown] = 16;
    m.Buttons[GamepadButton::DPadLeft] = 17;
    m.Buttons[GamepadButton::DPadRight] = 15;
    m.Buttons[GamepadButton::Center] = 13;
    m.Axes[GamepadAxis::LeftStickHori].Axis = 0;
    m.Axes[GamepadAxis::LeftStickVert].Axis = 1;
    m.Axes[GamepadAxis::RightStickHori].Axis = 2;
    m.Axes[GamepadAxis::RightStickVert].Axis = 3;
    m.Axes[GamepadAxis::LeftTrigger].Axis = 4;
    m.Axes[GamepadAxis::LeftTrigger].Scale = 0.5f;
    m.Axes[GamepadAxis::LeftTrigger].Bias = 0.5f;
    m.Axes[GamepadAxis::RightTrigger].Axis = 5;
    m.Axes[GamepadAxis::RightTrigger].Scale = 0.5f;
    m.Axes[GamepadAxis::RightTrigger].Bias = 0.5f;
    this->defaultGamepadMapping = m;
    #elif ORYOL_LINUX
    // Xbox360 wired controller is default
    // FIXME: triggers are not exposed as buttons, only as axes, DPad also only axes
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

    // DS4 Linux, FIXME DPad is not exposed as buttons, only axes :/
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
    m.Buttons[GamepadButton::Center] = 13;
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
    this->addGamepadMapping("Sony Computer Entertainment Wireless Controller", m);
    #elif ORYOL_WINDOWS
    // Xbox360 controller is the default mapping
    // FIXME: triggers are only exposed as axes 
    m.Buttons[GamepadButton::A] = 0;
    m.Buttons[GamepadButton::B] = 1;
    m.Buttons[GamepadButton::X] = 2;
    m.Buttons[GamepadButton::Y] = 3;
    m.Buttons[GamepadButton::LeftBumper] = 4;
    m.Buttons[GamepadButton::RightBumper] = 5;
    m.Buttons[GamepadButton::Back] = 6;
    m.Buttons[GamepadButton::Start] = 7;
    m.Buttons[GamepadButton::LeftStick] = 8;
    m.Buttons[GamepadButton::RightStick] = 9;
    m.Buttons[GamepadButton::DPadUp] = 10;
    m.Buttons[GamepadButton::DPadDown] = 12;
    m.Buttons[GamepadButton::DPadLeft] = 13;
    m.Buttons[GamepadButton::DPadRight] = 11;
    m.Axes[GamepadAxis::LeftStickHori].Axis = 0;
    m.Axes[GamepadAxis::LeftStickVert].Axis = 1;
    m.Axes[GamepadAxis::LeftStickVert].Scale = -1.0f;
    m.Axes[GamepadAxis::RightStickHori].Axis = 2;
    m.Axes[GamepadAxis::RightStickVert].Axis = 3;
    m.Axes[GamepadAxis::RightStickVert].Scale = -1.0f;
    m.Axes[GamepadAxis::LeftTrigger].Axis = 4;
    m.Axes[GamepadAxis::LeftTrigger].Scale = 0.5f;
    m.Axes[GamepadAxis::LeftTrigger].Bias = 0.5f;
    m.Axes[GamepadAxis::RightTrigger].Axis = 5;
    m.Axes[GamepadAxis::RightTrigger].Scale = 0.5f;
    m.Axes[GamepadAxis::RightTrigger].Bias = 0.5f;
    this->defaultGamepadMapping = m;

    // PS4 controller
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
    m.Buttons[GamepadButton::DPadUp] = 14;
    m.Buttons[GamepadButton::DPadDown] = 16;
    m.Buttons[GamepadButton::DPadLeft] = 17;
    m.Buttons[GamepadButton::DPadRight] = 15;
    m.Buttons[GamepadButton::Center] = 13;
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
    this->addGamepadMapping("Wireless Controller", m);
    #endif
}

//------------------------------------------------------------------------------
void
glfwInputMgr::updateGamepads() {
    for (int padIndex = 0; padIndex < inputDefs::maxNumGamepads; padIndex++) {
        auto& pad = this->gamepad[padIndex];
        bool present = 0 != glfwJoystickPresent(padIndex);
        if (present && !pad.attached) {
            // pad has just been attached
            pad.id = glfwGetJoystickName(padIndex);
            pad.mapping = this->lookupGamepadMapping(pad.id);
            pad.pressed = 0;
            pad.axes.Fill(0.0f);
        }
        else if (!present && pad.attached) {
            // pad has just been detached
            pad.id.Clear();
            pad.pressed = 0;
            pad.axes.Fill(0.0f);
        }
        pad.attached = present;
        if (pad.attached) {
            int numRawBtns = 0;
            const unsigned char* rawBtns = glfwGetJoystickButtons(padIndex, &numRawBtns);
            if (numRawBtns > inputDefs::maxNumRawButtons) {
                numRawBtns = inputDefs::maxNumRawButtons;
            }
            for (int rawBtnIndex = 0; rawBtnIndex < numRawBtns; rawBtnIndex++) {
                uint32_t mask = (1<<rawBtnIndex);
                if (rawBtns[rawBtnIndex] == GLFW_PRESS) {
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
            int numRawAxes = 0;
            const float* rawAxes = glfwGetJoystickAxes(padIndex, &numRawAxes);
            if (numRawAxes > inputDefs::maxNumRawAxes) {
                numRawAxes = inputDefs::maxNumRawAxes;
            }
            for (int rawAxisIndex = 0; rawAxisIndex < numRawAxes; rawAxisIndex++) {
                pad.axes[rawAxisIndex] = rawAxes[rawAxisIndex];
            }
        }
    }
}

//------------------------------------------------------------------------------
void
glfwInputMgr::keyCallback(GLFWwindow* win, int glfwKey, int /*glfwScancode*/, int glfwAction, int /*glfwMods*/) {
    if (nullptr != self) {
        Key::Code key = self->mapKey(glfwKey);
        if (Key::InvalidKey != key) {
            if (glfwAction == GLFW_PRESS) {
                self->keyboard.onKeyDown(key);
            }
            else if (glfwAction == GLFW_RELEASE) {
                self->keyboard.onKeyUp(key);
            }
            else {
                self->keyboard.onKeyRepeat(key);
            }
        }
    }
}

//------------------------------------------------------------------------------
void
glfwInputMgr::charCallback(GLFWwindow* win, unsigned int unicode) {
    if (nullptr != self) {
        self->keyboard.onChar((wchar_t)unicode);
    }
}

//------------------------------------------------------------------------------
void
glfwInputMgr::mouseButtonCallback(GLFWwindow* win, int glfwButton, int glfwAction, int glfwMods) {
    if (nullptr != self) {
        MouseButton::Code btn;
        switch (glfwButton) {
            case GLFW_MOUSE_BUTTON_LEFT:    btn = MouseButton::Left; break;
            case GLFW_MOUSE_BUTTON_RIGHT:   btn = MouseButton::Right; break;
            case GLFW_MOUSE_BUTTON_MIDDLE:  btn = MouseButton::Middle; break;
            default:                        btn = MouseButton::InvalidMouseButton; break;
        }
        if (btn != MouseButton::InvalidMouseButton) {
            PointerLockMode::Code lockMode = PointerLockMode::DontCare;
            if (glfwAction == GLFW_PRESS) {
                lockMode = self->mouse.onButtonDown(btn);
            }
            else if (glfwAction == GLFW_RELEASE) {
                lockMode = self->mouse.onButtonUp(btn);
            }
            if (PointerLockMode::Enable == lockMode) {
                GLFWwindow* glfwWindow = _priv::glfwDisplayMgr::getGlfwWindow();
                glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            else if (PointerLockMode::Disable == lockMode) {
                GLFWwindow* glfwWindow = _priv::glfwDisplayMgr::getGlfwWindow();
                glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
        }
    }
}

//------------------------------------------------------------------------------
void
glfwInputMgr::cursorPosCallback(GLFWwindow* win, double glfwX, double glfwY) {
    if (nullptr != self) {
        const glm::vec2 pos((float)glfwX, (float)glfwY);
        self->mouse.onPosMov(pos);
    }
}

//------------------------------------------------------------------------------
void
glfwInputMgr::scrollCallback(GLFWwindow* win, double glfwX, double glfwY) {
    if (nullptr != self) {
        const glm::vec2 scroll((float)glfwX, (float)glfwY);
        self->mouse.onScroll(scroll);
    }
}

//------------------------------------------------------------------------------
void
glfwInputMgr::cursorEnterCallback(GLFWwindow* win, int entered) {
    // FIXME?
}

//------------------------------------------------------------------------------
Key::Code
glfwInputMgr::mapKey(int glfwKey) const {
    if ((glfwKey >= 0) && (glfwKey <= GLFW_KEY_LAST)) {
        return keyTable[glfwKey];
    }
    else {
        return Key::InvalidKey;
    }
}

//------------------------------------------------------------------------------
void
glfwInputMgr::setupKeyTable() {

    for (int i = 0; i <= GLFW_KEY_LAST; i++) {
        keyTable[i] = Key::InvalidKey;
    }
    
    keyTable[GLFW_KEY_SPACE]          = Key::Space;
    keyTable[GLFW_KEY_APOSTROPHE]     = Key::Apostrophe;
    keyTable[GLFW_KEY_COMMA]          = Key::Comma;
    keyTable[GLFW_KEY_MINUS]          = Key::Minus;
    keyTable[GLFW_KEY_PERIOD]         = Key::Period;
    keyTable[GLFW_KEY_SLASH]          = Key::Slash;
    keyTable[GLFW_KEY_0]              = Key::N0;
    keyTable[GLFW_KEY_1]              = Key::N1;
    keyTable[GLFW_KEY_2]              = Key::N2;
    keyTable[GLFW_KEY_3]              = Key::N3;
    keyTable[GLFW_KEY_4]              = Key::N4;
    keyTable[GLFW_KEY_5]              = Key::N5;
    keyTable[GLFW_KEY_6]              = Key::N6;
    keyTable[GLFW_KEY_7]              = Key::N7;
    keyTable[GLFW_KEY_8]              = Key::N8;
    keyTable[GLFW_KEY_9]              = Key::N9;
    keyTable[GLFW_KEY_SEMICOLON]      = Key::Semicolon;
    keyTable[GLFW_KEY_EQUAL]          = Key::Equal;
    keyTable[GLFW_KEY_A]              = Key::A;
    keyTable[GLFW_KEY_B]              = Key::B;
    keyTable[GLFW_KEY_C]              = Key::C;
    keyTable[GLFW_KEY_D]              = Key::D;
    keyTable[GLFW_KEY_E]              = Key::E;
    keyTable[GLFW_KEY_F]              = Key::F;
    keyTable[GLFW_KEY_G]              = Key::G;
    keyTable[GLFW_KEY_H]              = Key::H;
    keyTable[GLFW_KEY_I]              = Key::I;
    keyTable[GLFW_KEY_J]              = Key::J;
    keyTable[GLFW_KEY_K]              = Key::K;
    keyTable[GLFW_KEY_L]              = Key::L;
    keyTable[GLFW_KEY_M]              = Key::M;
    keyTable[GLFW_KEY_N]              = Key::N;
    keyTable[GLFW_KEY_O]              = Key::O;
    keyTable[GLFW_KEY_P]              = Key::P;
    keyTable[GLFW_KEY_Q]              = Key::Q;
    keyTable[GLFW_KEY_R]              = Key::R;
    keyTable[GLFW_KEY_S]              = Key::S;
    keyTable[GLFW_KEY_T]              = Key::T;
    keyTable[GLFW_KEY_U]              = Key::U;
    keyTable[GLFW_KEY_V]              = Key::V;
    keyTable[GLFW_KEY_W]              = Key::W;
    keyTable[GLFW_KEY_X]              = Key::X;
    keyTable[GLFW_KEY_Y]              = Key::Y;
    keyTable[GLFW_KEY_Z]              = Key::Z;
    keyTable[GLFW_KEY_LEFT_BRACKET]   = Key::LeftBracket;
    keyTable[GLFW_KEY_BACKSLASH]      = Key::BackSlash;
    keyTable[GLFW_KEY_RIGHT_BRACKET]  = Key::RightBracket;
    keyTable[GLFW_KEY_GRAVE_ACCENT]   = Key::GraveAccent;
    keyTable[GLFW_KEY_WORLD_1]        = Key::World1;
    keyTable[GLFW_KEY_WORLD_2]        = Key::World2;
    keyTable[GLFW_KEY_ESCAPE]         = Key::Escape;
    keyTable[GLFW_KEY_ENTER]          = Key::Enter;
    keyTable[GLFW_KEY_TAB]            = Key::Tab;
    keyTable[GLFW_KEY_BACKSPACE]      = Key::BackSpace;
    keyTable[GLFW_KEY_INSERT]         = Key::Insert;
    keyTable[GLFW_KEY_DELETE]         = Key::Delete;
    keyTable[GLFW_KEY_RIGHT]          = Key::Right;
    keyTable[GLFW_KEY_LEFT]           = Key::Left;
    keyTable[GLFW_KEY_DOWN]           = Key::Down;
    keyTable[GLFW_KEY_UP]             = Key::Up;
    keyTable[GLFW_KEY_PAGE_UP]        = Key::PageUp;
    keyTable[GLFW_KEY_PAGE_DOWN]      = Key::PageDown;
    keyTable[GLFW_KEY_HOME]           = Key::Home;
    keyTable[GLFW_KEY_END]            = Key::End;
    keyTable[GLFW_KEY_CAPS_LOCK]      = Key::CapsLock;
    keyTable[GLFW_KEY_SCROLL_LOCK]    = Key::ScrollLock;
    keyTable[GLFW_KEY_NUM_LOCK]       = Key::NumLock;
    keyTable[GLFW_KEY_PRINT_SCREEN]   = Key::PrintScreen;
    keyTable[GLFW_KEY_PAUSE]          = Key::Pause;
    keyTable[GLFW_KEY_F1]             = Key::F1;
    keyTable[GLFW_KEY_F2]             = Key::F2;
    keyTable[GLFW_KEY_F3]             = Key::F3;
    keyTable[GLFW_KEY_F4]             = Key::F4;
    keyTable[GLFW_KEY_F5]             = Key::F5;
    keyTable[GLFW_KEY_F6]             = Key::F6;
    keyTable[GLFW_KEY_F7]             = Key::F7;
    keyTable[GLFW_KEY_F8]             = Key::F8;
    keyTable[GLFW_KEY_F9]             = Key::F9;
    keyTable[GLFW_KEY_F10]            = Key::F10;
    keyTable[GLFW_KEY_F11]            = Key::F11;
    keyTable[GLFW_KEY_F12]            = Key::F12;
    keyTable[GLFW_KEY_F13]            = Key::F13;
    keyTable[GLFW_KEY_F14]            = Key::F14;
    keyTable[GLFW_KEY_F15]            = Key::F15;
    keyTable[GLFW_KEY_F16]            = Key::F16;
    keyTable[GLFW_KEY_F17]            = Key::F17;
    keyTable[GLFW_KEY_F18]            = Key::F18;
    keyTable[GLFW_KEY_F19]            = Key::F19;
    keyTable[GLFW_KEY_F20]            = Key::F20;
    keyTable[GLFW_KEY_F21]            = Key::F21;
    keyTable[GLFW_KEY_F22]            = Key::F22;
    keyTable[GLFW_KEY_F23]            = Key::F23;
    keyTable[GLFW_KEY_F24]            = Key::F24;
    keyTable[GLFW_KEY_F25]            = Key::F25;
    keyTable[GLFW_KEY_KP_0]           = Key::Num0;
    keyTable[GLFW_KEY_KP_1]           = Key::Num1;
    keyTable[GLFW_KEY_KP_2]           = Key::Num2;
    keyTable[GLFW_KEY_KP_3]           = Key::Num3;
    keyTable[GLFW_KEY_KP_4]           = Key::Num4;
    keyTable[GLFW_KEY_KP_5]           = Key::Num5;
    keyTable[GLFW_KEY_KP_6]           = Key::Num6;
    keyTable[GLFW_KEY_KP_7]           = Key::Num7;
    keyTable[GLFW_KEY_KP_8]           = Key::Num8;
    keyTable[GLFW_KEY_KP_9]           = Key::Num9;
    keyTable[GLFW_KEY_KP_DECIMAL]     = Key::NumDecimal;
    keyTable[GLFW_KEY_KP_DIVIDE]      = Key::NumDivide;
    keyTable[GLFW_KEY_KP_MULTIPLY]    = Key::NumMultiply;
    keyTable[GLFW_KEY_KP_SUBTRACT]    = Key::NumSubtract;
    keyTable[GLFW_KEY_KP_ADD]         = Key::NumAdd;
    keyTable[GLFW_KEY_KP_ENTER]       = Key::NumEnter;
    keyTable[GLFW_KEY_KP_EQUAL]       = Key::NumEqual;
    keyTable[GLFW_KEY_LEFT_SHIFT]     = Key::LeftShift;
    keyTable[GLFW_KEY_LEFT_CONTROL]   = Key::LeftControl;
    keyTable[GLFW_KEY_LEFT_ALT]       = Key::LeftAlt;
    keyTable[GLFW_KEY_LEFT_SUPER]     = Key::LeftSuper;
    keyTable[GLFW_KEY_RIGHT_SHIFT]    = Key::RightShift;
    keyTable[GLFW_KEY_RIGHT_CONTROL]  = Key::RightControl;
    keyTable[GLFW_KEY_RIGHT_ALT]      = Key::RightAlt;
    keyTable[GLFW_KEY_RIGHT_SUPER]    = Key::RightSuper;
    keyTable[GLFW_KEY_MENU]           = Key::Menu;
}

} // namespace _priv
} // namespace Oryol
