//------------------------------------------------------------------------------
//  glfwInputMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glfwInputMgr.h"
#include "Gfx/glfw/glfwDisplayMgr.h"
#include "Core/Core.h"
#include "Core/RunLoop.h"
#include "GLFW/glfw3.h"
#include "Input/InputProtocol.h"

namespace Oryol {
namespace _priv {
    
glfwInputMgr* glfwInputMgr::self = nullptr;

static Key::Code keyTable[GLFW_KEY_LAST + 1];

//------------------------------------------------------------------------------
glfwInputMgr::glfwInputMgr() :
runLoopId(RunLoop::InvalidId) {
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
    
    inputMgrBase::setup(setup);
    this->keyboard.Attached = true;
    this->mouse.Attached = true;
    
    // first check that the Gfx module has already been initialized
    GLFWwindow* glfwWindow = _priv::glfwDisplayMgr::getGlfwWindow();
    if (nullptr == glfwWindow) {
        o_error("glfwInputMgr: Gfx::Setup must be called before Input::Setup!\n");
        return;
    }
    this->setupKeyTable();
    this->setupCallbacks(glfwWindow);
    this->setCursorMode(CursorMode::Normal);
    
    // attach our reset callback to the global runloop
    this->runLoopId = Core::PostRunLoop()->Add([this]() { this->reset(); });    
}

//------------------------------------------------------------------------------
void
glfwInputMgr::discard() {
    
    // remove glfw input callbacks
    GLFWwindow* glfwWindow = _priv::glfwDisplayMgr::getGlfwWindow();
    o_assert(nullptr != glfwWindow);
    this->discardCallbacks(glfwWindow);
    
    // detach our reset callback from runloop
    Core::PostRunLoop()->Remove(this->runLoopId);
    this->runLoopId = RunLoop::InvalidId;
    
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
glfwInputMgr::setCursorMode(CursorMode::Code newMode) {
    if (newMode != this->cursorMode) {
        int glfwInputMode;
        switch (newMode) {
            case CursorMode::Normal:    glfwInputMode = GLFW_CURSOR_NORMAL; break;
            case CursorMode::Hidden:    glfwInputMode = GLFW_CURSOR_HIDDEN; break;
            default:                    glfwInputMode = GLFW_CURSOR_DISABLED; break;
        }
        GLFWwindow* glfwWindow = _priv::glfwDisplayMgr::getGlfwWindow();
        o_assert(nullptr != glfwWindow);
        glfwSetInputMode(glfwWindow, GLFW_CURSOR, glfwInputMode);
    }
    inputMgrBase::setCursorMode(newMode);
}

//------------------------------------------------------------------------------
void
glfwInputMgr::reset() {
    for (int32 i = 0; i < MaxNumGamepads; i++) {
        this->gamepads[i].Attached = glfwJoystickPresent(i) != 0;
    }
    inputMgrBase::reset();
}
    
//------------------------------------------------------------------------------
void
glfwInputMgr::keyCallback(GLFWwindow* win, int glfwKey, int /*glfwScancode*/, int glfwAction, int /*glfwMods*/) {
    if (nullptr != self) {
        Key::Code key = self->mapKey(glfwKey);
        if (Key::InvalidKey != key) {
            auto msg = InputProtocol::KeyEvent::Create();
            msg->Key = key;
            if (glfwAction == GLFW_PRESS) {
                self->keyboard.onKeyDown(key);
                msg->Down = true;
            }
            else if (glfwAction == GLFW_RELEASE) {
                self->keyboard.onKeyUp(key);
                msg->Up = true;
            }
            else {
                self->keyboard.onKeyRepeat(key);
                msg->Repeat = true;
            }
            self->notifyHandlers(msg);
        }
    }
}

//------------------------------------------------------------------------------
void
glfwInputMgr::charCallback(GLFWwindow* win, unsigned int unicode) {
    if (nullptr != self) {
        self->keyboard.onChar((wchar_t)unicode);
        auto msg = InputProtocol::WCharEvent::Create();
        msg->WChar = (wchar_t) unicode;
        self->notifyHandlers(msg);
    }
}

//------------------------------------------------------------------------------
void
glfwInputMgr::mouseButtonCallback(GLFWwindow* win, int glfwButton, int glfwAction, int glfwMods) {
    if (nullptr != self) {
        Mouse::Button btn;
        switch (glfwButton) {
            case GLFW_MOUSE_BUTTON_LEFT:    btn = Mouse::LMB; break;
            case GLFW_MOUSE_BUTTON_RIGHT:   btn = Mouse::RMB; break;
            case GLFW_MOUSE_BUTTON_MIDDLE:  btn = Mouse::MMB; break;
            default:                        btn = Mouse::InvalidButton; break;
        }
        if (btn != Mouse::InvalidButton) {
            auto msg = InputProtocol::MouseButtonEvent::Create();
            msg->MouseButton = btn;
            if (glfwAction == GLFW_PRESS) {
                self->mouse.onButtonDown(btn);
                msg->Down = true;
            }
            else if (glfwAction == GLFW_RELEASE) {
                self->mouse.onButtonUp(btn);
                msg->Up = true;
            }
            self->notifyHandlers(msg);
        }
    }
}

//------------------------------------------------------------------------------
void
glfwInputMgr::cursorPosCallback(GLFWwindow* win, double glfwX, double glfwY) {
    if (nullptr != self) {
        const glm::vec2 pos((float32)glfwX, (float32)glfwY);
        self->mouse.onPosMov(pos);
        auto msg = InputProtocol::MouseMoveEvent::Create();
        msg->Movement = self->mouse.Movement;
        msg->Position = self->mouse.Position;
        self->notifyHandlers(msg);
    }
}

//------------------------------------------------------------------------------
void
glfwInputMgr::scrollCallback(GLFWwindow* win, double glfwX, double glfwY) {
    if (nullptr != self) {
        const glm::vec2 scroll((float32)glfwX, (float32)glfwY);
        self->mouse.Scroll = scroll;
        auto msg = InputProtocol::MouseScrollEvent::Create();
        msg->Scroll = scroll;
        self->notifyHandlers(msg);
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

    for (int32 i = 0; i <= GLFW_KEY_LAST; i++) {
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
