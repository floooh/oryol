//------------------------------------------------------------------------------
//  winInputMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "winInputMgr.h"
#include "Core/Core.h"
#include "Core/RunLoop.h"
#include "Gfx/private/win/winDisplayMgr.h"

namespace Oryol {
namespace _priv {

winInputMgr* winInputMgr::self = nullptr;

static Key::Code keyTable[ORYOL_WIN_KEY_LAST + 1];

//------------------------------------------------------------------------------
winInputMgr::winInputMgr() :
runLoopId(RunLoop::InvalidId) {
    o_assert(nullptr == self);
    self = this;
}

//------------------------------------------------------------------------------
winInputMgr::~winInputMgr() {
    o_assert(nullptr != self);
    self = nullptr;
}

//------------------------------------------------------------------------------
void
winInputMgr::setup(const InputSetup& setup) {

    inputMgrBase::setup(setup);
    this->keyboard.attached = true;
    this->mouse.attached = true;

    // first check that the Gfx module has already been initialized
    // (directly access the 'private' interface of winDisplayMgr, 
    // better don't do this in regular code!)
    if (!winDisplayMgr::self && winDisplayMgr::self->IsDisplayValid()) {
        o_error("winInputMgr: Gfx::Setup() must be called before Input::Setup!\n");
        return;
    }
    this->setupKeyTable();
    this->setupCallbacks();

    // attach our reset callback to the global runloop
    this->runLoopId = Core::PostRunLoop()->Add([this]() { this->reset(); });
}

//------------------------------------------------------------------------------
void
winInputMgr::discard() {
    this->discardCallbacks();
    Core::PostRunLoop()->Remove(this->runLoopId);
    this->runLoopId = RunLoop::InvalidId;
    inputMgrBase::discard();
}

//------------------------------------------------------------------------------
void
winInputMgr::setupCallbacks() {
    o_assert(winDisplayMgr::self);
    winDisplayMgr::callbackTable& cb = winDisplayMgr::self->callbacks;
    cb.key = keyCallback;
    cb.character = charCallback;
    cb.mouseButton = mouseButtonCallback;
    cb.cursorPos = cursorPosCallback;
    cb.cursorEnter = cursorEnterCallback;
    cb.scroll = scrollCallback;
}

//------------------------------------------------------------------------------
void
winInputMgr::discardCallbacks() {
    o_assert(winDisplayMgr::self);
    winDisplayMgr::callbackTable& cb = winDisplayMgr::self->callbacks;
    cb.key = nullptr;
    cb.character = nullptr;
    cb.mouseButton = nullptr;
    cb.cursorPos = nullptr;
    cb.cursorEnter = nullptr;
    cb.scroll = nullptr;
}

//------------------------------------------------------------------------------
void
winInputMgr::keyCallback(int winKey, int /*winScancode*/, int winAction, int /*winMods*/) {
    if (nullptr != self) {
        Key::Code key = self->mapKey(winKey);
        if (Key::InvalidKey != key) {
            if (winAction == ORYOL_WIN_PRESS) {
                self->keyboard.onKeyDown(key);
            }
            else if (winAction == ORYOL_WIN_RELEASE) {
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
winInputMgr::charCallback(unsigned int unicode) {
    if (nullptr != self) {
        self->keyboard.onChar((wchar_t)unicode);
    }
}

//------------------------------------------------------------------------------
void
winInputMgr::mouseButtonCallback(int winButton, int winAction, int winMods) {
    if (nullptr != self) {
        MouseButton::Code btn;
        switch (winButton) {
            case ORYOL_WIN_MOUSE_BUTTON_LEFT:    btn = MouseButton::Left; break;
            case ORYOL_WIN_MOUSE_BUTTON_RIGHT:   btn = MouseButton::Right; break;
            case ORYOL_WIN_MOUSE_BUTTON_MIDDLE:  btn = MouseButton::Middle; break;
            default:                             btn = MouseButton::InvalidMouseButton; break;
        }
        if (btn != MouseButton::InvalidMouseButton) {
            PointerLockMode::Code lockMode = PointerLockMode::DontCare;
            if (winAction == ORYOL_WIN_PRESS) {
                lockMode = self->mouse.onButtonDown(btn);
            }
            else if (winAction == ORYOL_WIN_RELEASE) {
                lockMode = self->mouse.onButtonUp(btn);
            }
            if (PointerLockMode::Enable == lockMode) {
                winDisplayMgr::self->setInputMode(ORYOL_WIN_CURSOR, ORYOL_WIN_CURSOR_DISABLED);
            }
            else if (PointerLockMode::Disable == lockMode) {
                winDisplayMgr::self->setInputMode(ORYOL_WIN_CURSOR, ORYOL_WIN_CURSOR_NORMAL);
            }
        }
    }
}

//------------------------------------------------------------------------------
void
winInputMgr::cursorPosCallback(double winX, double winY) {
    if (nullptr != self) {
        const glm::vec2 pos((float)winX, (float)winY);
        self->mouse.onPosMov(pos);
    }
}

//------------------------------------------------------------------------------
void
winInputMgr::scrollCallback(double winX, double winY) {
    if (nullptr != self) {
        const glm::vec2 scroll((float)winX, (float)winY);
        self->mouse.scroll = scroll;
    }
}

//------------------------------------------------------------------------------
void
winInputMgr::cursorEnterCallback(int entered) {
    // empty
}

//------------------------------------------------------------------------------
Key::Code
winInputMgr::mapKey(int winKey) const {
    if ((winKey >= 0) && (winKey <= ORYOL_WIN_KEY_LAST)) {
        return keyTable[winKey];
    }
    else {
        return Key::InvalidKey;
    }
}

//------------------------------------------------------------------------------
void
winInputMgr::setupKeyTable() {

    for (int i = 0; i <= ORYOL_WIN_KEY_LAST; i++) {
        keyTable[i] = Key::InvalidKey;
    }

    keyTable[ORYOL_WIN_KEY_SPACE] = Key::Space;
    keyTable[ORYOL_WIN_KEY_APOSTROPHE] = Key::Apostrophe;
    keyTable[ORYOL_WIN_KEY_COMMA] = Key::Comma;
    keyTable[ORYOL_WIN_KEY_MINUS] = Key::Minus;
    keyTable[ORYOL_WIN_KEY_PERIOD] = Key::Period;
    keyTable[ORYOL_WIN_KEY_SLASH] = Key::Slash;
    keyTable[ORYOL_WIN_KEY_0] = Key::N0;
    keyTable[ORYOL_WIN_KEY_1] = Key::N1;
    keyTable[ORYOL_WIN_KEY_2] = Key::N2;
    keyTable[ORYOL_WIN_KEY_3] = Key::N3;
    keyTable[ORYOL_WIN_KEY_4] = Key::N4;
    keyTable[ORYOL_WIN_KEY_5] = Key::N5;
    keyTable[ORYOL_WIN_KEY_6] = Key::N6;
    keyTable[ORYOL_WIN_KEY_7] = Key::N7;
    keyTable[ORYOL_WIN_KEY_8] = Key::N8;
    keyTable[ORYOL_WIN_KEY_9] = Key::N9;
    keyTable[ORYOL_WIN_KEY_SEMICOLON] = Key::Semicolon;
    keyTable[ORYOL_WIN_KEY_EQUAL] = Key::Equal;
    keyTable[ORYOL_WIN_KEY_A] = Key::A;
    keyTable[ORYOL_WIN_KEY_B] = Key::B;
    keyTable[ORYOL_WIN_KEY_C] = Key::C;
    keyTable[ORYOL_WIN_KEY_D] = Key::D;
    keyTable[ORYOL_WIN_KEY_E] = Key::E;
    keyTable[ORYOL_WIN_KEY_F] = Key::F;
    keyTable[ORYOL_WIN_KEY_G] = Key::G;
    keyTable[ORYOL_WIN_KEY_H] = Key::H;
    keyTable[ORYOL_WIN_KEY_I] = Key::I;
    keyTable[ORYOL_WIN_KEY_J] = Key::J;
    keyTable[ORYOL_WIN_KEY_K] = Key::K;
    keyTable[ORYOL_WIN_KEY_L] = Key::L;
    keyTable[ORYOL_WIN_KEY_M] = Key::M;
    keyTable[ORYOL_WIN_KEY_N] = Key::N;
    keyTable[ORYOL_WIN_KEY_O] = Key::O;
    keyTable[ORYOL_WIN_KEY_P] = Key::P;
    keyTable[ORYOL_WIN_KEY_Q] = Key::Q;
    keyTable[ORYOL_WIN_KEY_R] = Key::R;
    keyTable[ORYOL_WIN_KEY_S] = Key::S;
    keyTable[ORYOL_WIN_KEY_T] = Key::T;
    keyTable[ORYOL_WIN_KEY_U] = Key::U;
    keyTable[ORYOL_WIN_KEY_V] = Key::V;
    keyTable[ORYOL_WIN_KEY_W] = Key::W;
    keyTable[ORYOL_WIN_KEY_X] = Key::X;
    keyTable[ORYOL_WIN_KEY_Y] = Key::Y;
    keyTable[ORYOL_WIN_KEY_Z] = Key::Z;
    keyTable[ORYOL_WIN_KEY_LEFT_BRACKET] = Key::LeftBracket;
    keyTable[ORYOL_WIN_KEY_BACKSLASH] = Key::BackSlash;
    keyTable[ORYOL_WIN_KEY_RIGHT_BRACKET] = Key::RightBracket;
    keyTable[ORYOL_WIN_KEY_GRAVE_ACCENT] = Key::GraveAccent;
    keyTable[ORYOL_WIN_KEY_WORLD_1] = Key::World1;
    keyTable[ORYOL_WIN_KEY_WORLD_2] = Key::World2;
    keyTable[ORYOL_WIN_KEY_ESCAPE] = Key::Escape;
    keyTable[ORYOL_WIN_KEY_ENTER] = Key::Enter;
    keyTable[ORYOL_WIN_KEY_TAB] = Key::Tab;
    keyTable[ORYOL_WIN_KEY_BACKSPACE] = Key::BackSpace;
    keyTable[ORYOL_WIN_KEY_INSERT] = Key::Insert;
    keyTable[ORYOL_WIN_KEY_DELETE] = Key::Delete;
    keyTable[ORYOL_WIN_KEY_RIGHT] = Key::Right;
    keyTable[ORYOL_WIN_KEY_LEFT] = Key::Left;
    keyTable[ORYOL_WIN_KEY_DOWN] = Key::Down;
    keyTable[ORYOL_WIN_KEY_UP] = Key::Up;
    keyTable[ORYOL_WIN_KEY_PAGE_UP] = Key::PageUp;
    keyTable[ORYOL_WIN_KEY_PAGE_DOWN] = Key::PageDown;
    keyTable[ORYOL_WIN_KEY_HOME] = Key::Home;
    keyTable[ORYOL_WIN_KEY_END] = Key::End;
    keyTable[ORYOL_WIN_KEY_CAPS_LOCK] = Key::CapsLock;
    keyTable[ORYOL_WIN_KEY_SCROLL_LOCK] = Key::ScrollLock;
    keyTable[ORYOL_WIN_KEY_NUM_LOCK] = Key::NumLock;
    keyTable[ORYOL_WIN_KEY_PRINT_SCREEN] = Key::PrintScreen;
    keyTable[ORYOL_WIN_KEY_PAUSE] = Key::Pause;
    keyTable[ORYOL_WIN_KEY_F1] = Key::F1;
    keyTable[ORYOL_WIN_KEY_F2] = Key::F2;
    keyTable[ORYOL_WIN_KEY_F3] = Key::F3;
    keyTable[ORYOL_WIN_KEY_F4] = Key::F4;
    keyTable[ORYOL_WIN_KEY_F5] = Key::F5;
    keyTable[ORYOL_WIN_KEY_F6] = Key::F6;
    keyTable[ORYOL_WIN_KEY_F7] = Key::F7;
    keyTable[ORYOL_WIN_KEY_F8] = Key::F8;
    keyTable[ORYOL_WIN_KEY_F9] = Key::F9;
    keyTable[ORYOL_WIN_KEY_F10] = Key::F10;
    keyTable[ORYOL_WIN_KEY_F11] = Key::F11;
    keyTable[ORYOL_WIN_KEY_F12] = Key::F12;
    keyTable[ORYOL_WIN_KEY_F13] = Key::F13;
    keyTable[ORYOL_WIN_KEY_F14] = Key::F14;
    keyTable[ORYOL_WIN_KEY_F15] = Key::F15;
    keyTable[ORYOL_WIN_KEY_F16] = Key::F16;
    keyTable[ORYOL_WIN_KEY_F17] = Key::F17;
    keyTable[ORYOL_WIN_KEY_F18] = Key::F18;
    keyTable[ORYOL_WIN_KEY_F19] = Key::F19;
    keyTable[ORYOL_WIN_KEY_F20] = Key::F20;
    keyTable[ORYOL_WIN_KEY_F21] = Key::F21;
    keyTable[ORYOL_WIN_KEY_F22] = Key::F22;
    keyTable[ORYOL_WIN_KEY_F23] = Key::F23;
    keyTable[ORYOL_WIN_KEY_F24] = Key::F24;
    keyTable[ORYOL_WIN_KEY_F25] = Key::F25;
    keyTable[ORYOL_WIN_KEY_KP_0] = Key::Num0;
    keyTable[ORYOL_WIN_KEY_KP_1] = Key::Num1;
    keyTable[ORYOL_WIN_KEY_KP_2] = Key::Num2;
    keyTable[ORYOL_WIN_KEY_KP_3] = Key::Num3;
    keyTable[ORYOL_WIN_KEY_KP_4] = Key::Num4;
    keyTable[ORYOL_WIN_KEY_KP_5] = Key::Num5;
    keyTable[ORYOL_WIN_KEY_KP_6] = Key::Num6;
    keyTable[ORYOL_WIN_KEY_KP_7] = Key::Num7;
    keyTable[ORYOL_WIN_KEY_KP_8] = Key::Num8;
    keyTable[ORYOL_WIN_KEY_KP_9] = Key::Num9;
    keyTable[ORYOL_WIN_KEY_KP_DECIMAL] = Key::NumDecimal;
    keyTable[ORYOL_WIN_KEY_KP_DIVIDE] = Key::NumDivide;
    keyTable[ORYOL_WIN_KEY_KP_MULTIPLY] = Key::NumMultiply;
    keyTable[ORYOL_WIN_KEY_KP_SUBTRACT] = Key::NumSubtract;
    keyTable[ORYOL_WIN_KEY_KP_ADD] = Key::NumAdd;
    keyTable[ORYOL_WIN_KEY_KP_ENTER] = Key::NumEnter;
    keyTable[ORYOL_WIN_KEY_KP_EQUAL] = Key::NumEqual;
    keyTable[ORYOL_WIN_KEY_LEFT_SHIFT] = Key::LeftShift;
    keyTable[ORYOL_WIN_KEY_LEFT_CONTROL] = Key::LeftControl;
    keyTable[ORYOL_WIN_KEY_LEFT_ALT] = Key::LeftAlt;
    keyTable[ORYOL_WIN_KEY_LEFT_SUPER] = Key::LeftSuper;
    keyTable[ORYOL_WIN_KEY_RIGHT_SHIFT] = Key::RightShift;
    keyTable[ORYOL_WIN_KEY_RIGHT_CONTROL] = Key::RightControl;
    keyTable[ORYOL_WIN_KEY_RIGHT_ALT] = Key::RightAlt;
    keyTable[ORYOL_WIN_KEY_RIGHT_SUPER] = Key::RightSuper;
    keyTable[ORYOL_WIN_KEY_MENU] = Key::Menu;
}

} // namespace _priv
} // namespace Oryol
