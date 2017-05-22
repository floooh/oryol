//------------------------------------------------------------------------------
//  raspiInputMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "raspiInputMgr.h"
#include "Core/Core.h"
#include "Core/Log.h"
#include "Core/String/StringBuilder.h"
#include "Gfx/Gfx.h"
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
raspiInputMgr::raspiInputMgr() :
runLoopId(RunLoop::InvalidId),
kbdFd(-1),
mouseFd(-1),
leftShift(false),
rightShift(false),
maxMouseX(0),
maxMouseY(0),
curMouseX(0),
curMouseY(0) {
    // empty
}

//------------------------------------------------------------------------------
raspiInputMgr::~raspiInputMgr() {
    // empty
}

//------------------------------------------------------------------------------
void
raspiInputMgr::setup(const InputSetup& setup) {
    inputMgrBase::setup(setup);
    if (this->openDevices()) {
        if (-1 != this->kbdFd) {
            this->keyboard.attached = true;
        }
        if (-1 != this->mouseFd) {
            this->mouse.attached = true;
        }
    }
    this->runLoopId = Core::PreRunLoop()->Add([this]() { this->pollInput(); });
}

//------------------------------------------------------------------------------
void
raspiInputMgr::discard() {
    this->closeDevices();
    Core::PostRunLoop()->Remove(this->runLoopId);
    this->runLoopId = RunLoop::InvalidId;
    inputMgrBase::discard();
}

//------------------------------------------------------------------------------
bool
raspiInputMgr::openDevices() {
    o_assert_dbg(-1 == this->kbdFd);
    o_assert_dbg(-1 == this->mouseFd);

    // see http://www.peteronion.org.uk/PiProgs/input.c
    StringBuilder strBuilder;
    const char* dirName = "/dev/input/by-id";
    DIR* dirp = opendir(dirName);
    if (dirp) {
        dirent* dp;
        while (nullptr != (dp = readdir(dirp))) {
            if ((-1 == this->kbdFd) && strstr(dp->d_name, "-event-kbd")) {
                strBuilder.Format(1024, "%s/%s", dirName, dp->d_name);
                this->kbdFd = open(strBuilder.AsCStr(), O_RDONLY|O_NONBLOCK);
                if (-1 != this->kbdFd) {
                    ioctl(this->kbdFd, EVIOCGRAB, 1);
                }
            }
            else if ((-1 == this->mouseFd) && strstr(dp->d_name, "-event-mouse")) {
                strBuilder.Format(1024, "%s/%s", dirName, dp->d_name);
                this->mouseFd = open(strBuilder.AsCStr(), O_RDONLY|O_NONBLOCK);
                if (-1 != this->mouseFd) {
                    ioctl(this->mouseFd, EVIOCGRAB, 1);
                }
            }
        }
        closedir(dirp);
        return true;
    }
    else {
        Log::Warn("raspiInputMgr: failed to open /dev/input/by-id\n");
        return false;
    }
}

//------------------------------------------------------------------------------
void
raspiInputMgr::closeDevices() {
    if (-1 != this->kbdFd) {
        close(this->kbdFd);
        this->kbdFd = -1;
    }
    if (-1 != this->mouseFd) {
        close(this->mouseFd);
        this->mouseFd = -1;
    }
}

//------------------------------------------------------------------------------
void
raspiInputMgr::pollInput() {
    this->reset();
    this->updateMaxMouseCoords();

    static const int maxEvents = 64;
    struct input_event buf[maxEvents];
    if (-1 != this->kbdFd) {
        // poll keyboard input
        const int bytesRead = read(this->kbdFd, buf, sizeof(buf));
        if (bytesRead > 0) {
            const int numEvents = bytesRead / sizeof(input_event);
            for (int i = 0; i < numEvents; i++) {
                const input_event& event = buf[i];
                if (event.type == EV_KEY) {
                    this->onKey(event.code, event.value);
                }
            }
        }
    }
    if (-1 != this->mouseFd) {
        // poll mouse input
        const int bytesRead = read(this->mouseFd, buf, sizeof(buf));
        if (bytesRead > 0) {
            const int numEvents = bytesRead / sizeof(input_event);
            for (int i = 0; i < numEvents; i++) {
                const input_event& event = buf[i];
                if (event.type == EV_KEY) {
                    this->onMouseButton(event.code, event.value);
                }
                else if (event.type == EV_REL) {
                    this->onMouseMove(event.code, event.value);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
void
raspiInputMgr::updateMaxMouseCoords() {
    const DisplayAttrs& dispAttrs = Gfx::DisplayAttrs();
    this->maxMouseX = dispAttrs.FramebufferWidth * mouseSensitivity;
    this->maxMouseY = dispAttrs.FramebufferHeight * mouseSensitivity;
}

//------------------------------------------------------------------------------
void
raspiInputMgr::onKey(unsigned short code, int val) {
    // key codes
    const Key::Code key = this->mapKey(code);
    if (Key::InvalidKey != key) {
        switch (val) {
            case 0: this->keyboard.onKeyUp(key); break;
            case 1: this->keyboard.onKeyDown(key); break;
            case 2: this->keyboard.onKeyRepeat(key); break;
            default: /* can't happen */ break;
        }
    }
    // character code on key-down and -repeat
    if (code == KEY_LEFTSHIFT) {
        this->leftShift = (0 != val);
    }
    else if (code == KEY_RIGHTSHIFT) {
        this->rightShift = (0 != val);
    }
    if ((val == 1) || (val == 2)) {
        wchar_t wchr = this->mapChar(code, this->leftShift|this->rightShift);
        if (0 != wchr) {
            this->keyboard.onChar(wchr);
        }
    }
}

//------------------------------------------------------------------------------
void
raspiInputMgr::onMouseButton(unsigned short code, int val) {
    MouseButton::Code btn;
    switch (code) {
        case BTN_LEFT:      btn = MouseButton::Left; break;
        case BTN_RIGHT:     btn = MouseButton::Right; break;
        case BTN_MIDDLE:    btn = MouseButton::Middle; break;
        default:            btn = MouseButton::InvalidMouseButton; break;
    }
    if (MouseButton::InvalidMouseButton != btn) {
        if (1 == val) {
            this->mouse.onButtonDown(btn);
        }
        else if (0 == val) {
            this->mouse.onButtonUp(btn);
        }
    }
}

//------------------------------------------------------------------------------
void
raspiInputMgr::onMouseMove(unsigned short code, int val) {
    if (code < 2) {
        // X/Y movement
        if (0 == code) {
            this->curMouseX += val;
        }
        else if (1 == code) {
            this->curMouseY += val;
        }
        if (this->curMouseX < 0) {
            this->curMouseX = 0;
        }
        else if (this->curMouseX > this->maxMouseX) {
            this->curMouseX = this->maxMouseX;
        }
        if (this->curMouseY < 0) {
            this->curMouseY = 0;
        }
        else if (this->curMouseY > this->maxMouseY) {
            this->curMouseY = this->maxMouseY;
        }
        const glm::vec2 pos((float32)(this->curMouseX / mouseSensitivity),
                            (float32)(this->curMouseY / mouseSensitivity));
        this->mouse.onPosMov(pos);
    }
    else if (code == 8) {
        // mouse wheel, val is +1 or -1
        const glm::vec2 scroll(0.0f, (float32)val);
        this->mouse.onScroll(scroll);
    }
}

//------------------------------------------------------------------------------
Key::Code
raspiInputMgr::mapKey(unsigned short code) {
    switch (code) {
        case KEY_ESC:           return Key::Escape;
        case KEY_1:	            return Key::N1;
        case KEY_2:	            return Key::N2;
        case KEY_3:	            return Key::N3;
        case KEY_4:	            return Key::N4;
        case KEY_5:             return Key::N5;
        case KEY_6:             return Key::N6;
        case KEY_7:             return Key::N7;
        case KEY_8:             return Key::N8;
        case KEY_9:             return Key::N9;
        case KEY_0:             return Key::N0;
        case KEY_MINUS:	        return Key::Minus; 	
        case KEY_EQUAL:         return Key::Equal;
        case KEY_BACKSPACE:     return Key::BackSpace;
        case KEY_TAB:           return Key::Tab;
        case KEY_Q:             return Key::Q;
        case KEY_W:             return Key::W;
        case KEY_E:             return Key::E;
        case KEY_R:             return Key::R;
        case KEY_T:             return Key::T;
        case KEY_Y:             return Key::Y;
        case KEY_U:             return Key::U;
        case KEY_I:             return Key::I;
        case KEY_O:             return Key::O;
        case KEY_P:             return Key::P;
        case KEY_LEFTBRACE:     return Key::LeftBracket;
        case KEY_RIGHTBRACE:    return Key::RightBracket;
        case KEY_ENTER:         return Key::Enter;
        case KEY_LEFTCTRL:      return Key::LeftControl;
        case KEY_A:             return Key::A;
        case KEY_S:             return Key::S;
        case KEY_D:             return Key::D;
        case KEY_F:             return Key::F;
        case KEY_G:             return Key::G;
        case KEY_H:             return Key::H;
        case KEY_J:             return Key::J;
        case KEY_K:             return Key::K;
        case KEY_L:             return Key::L;
        case KEY_SEMICOLON:     return Key::Semicolon;
        case KEY_APOSTROPHE:    return Key::Apostrophe;
        case KEY_GRAVE:         return Key::GraveAccent;
        case KEY_LEFTSHIFT:     return Key::LeftShift;
        case KEY_BACKSLASH:     return Key::BackSlash;
        case KEY_Z:             return Key::Z;
        case KEY_X:             return Key::X;     
        case KEY_C:             return Key::C;
        case KEY_V:             return Key::V;
        case KEY_B:             return Key::B;
        case KEY_N:             return Key::N;
        case KEY_M:             return Key::M;
        case KEY_COMMA:         return Key::Comma;
        case KEY_DOT:           return Key::Period;
        case KEY_SLASH:         return Key::Slash;
        case KEY_RIGHTSHIFT:    return Key::RightShift;
        case KEY_KPASTERISK:    return Key::NumMultiply; 
        case KEY_LEFTALT:       return Key::LeftAlt;
        case KEY_SPACE:         return Key::Space;
        case KEY_CAPSLOCK:      return Key::CapsLock;
        case KEY_F1:            return Key::F1;
        case KEY_F2:            return Key::F2;
        case KEY_F3:            return Key::F3;
        case KEY_F4:            return Key::F4;
        case KEY_F5:            return Key::F5;
        case KEY_F6:            return Key::F6;
        case KEY_F7:            return Key::F7;
        case KEY_F8:            return Key::F8;
        case KEY_F9:            return Key::F9;
        case KEY_F10:           return Key::F10;
        case KEY_NUMLOCK:       return Key::NumLock;
        case KEY_SCROLLLOCK:    return Key::ScrollLock;
        case KEY_KP7:           return Key::Num7;
        case KEY_KP8:           return Key::Num8;
        case KEY_KP9:           return Key::Num9;
        case KEY_KPMINUS:       return Key::NumSubtract;
        case KEY_KP4:           return Key::Num4;
        case KEY_KP5:           return Key::Num5;
        case KEY_KP6:           return Key::Num6;
        case KEY_KPPLUS:        return Key::NumAdd;
        case KEY_KP1:           return Key::Num1;
        case KEY_KP2:           return Key::Num2;
        case KEY_KP3:           return Key::Num3;
        case KEY_KP0:           return Key::Num0;
        case KEY_KPDOT:         return Key::NumDecimal; 
        case KEY_F11:           return Key::F11;
        case KEY_F12:           return Key::F12;
        case KEY_KPENTER:       return Key::NumEnter;
        case KEY_RIGHTCTRL:     return Key::RightControl;
        case KEY_KPSLASH:       return Key::NumDivide;
        case KEY_RIGHTALT:      return Key::RightAlt;
        case KEY_HOME:          return Key::Home;
        case KEY_UP:            return Key::Up;
        case KEY_PAGEUP:        return Key::PageUp;
        case KEY_LEFT:          return Key::Left;
        case KEY_RIGHT:         return Key::Right;
        case KEY_END:           return Key::End;
        case KEY_DOWN:          return Key::Down;
        case KEY_PAGEDOWN:      return Key::PageDown;
        case KEY_INSERT:        return Key::Insert;
        case KEY_DELETE:        return Key::Delete;
        case KEY_KPEQUAL:       return Key::NumEqual;
        case KEY_PAUSE:         return Key::Pause;
        case KEY_LEFTMETA:      return Key::LeftSuper;
        case KEY_RIGHTMETA:     return Key::RightSuper;
        case KEY_MENU:          return Key::Menu;
        case KEY_F13:           return Key::F13;
        case KEY_F14:           return Key::F14;
        case KEY_F15:           return Key::F15;
        case KEY_F16:           return Key::F16;
        case KEY_F17:           return Key::F17;
        case KEY_F18:           return Key::F18;
        case KEY_F19:           return Key::F19;
        case KEY_F20:           return Key::F20;
        case KEY_F21:           return Key::F21;
        case KEY_F22:           return Key::F22;
        case KEY_F23:           return Key::F23;
        case KEY_F24:           return Key::F24;
        default:                return Key::InvalidKey;
    }
}

//------------------------------------------------------------------------------
wchar_t
raspiInputMgr::mapChar(unsigned short code, bool shift) {

    // this is hardwired to the US keyboard layout
    if (shift) {
        switch (code) {
            case KEY_1:	            return L'!';
            case KEY_2:	            return L'@';
            case KEY_3:	            return L'#';
            case KEY_4:	            return L'$';
            case KEY_5:             return L'%';
            case KEY_6:             return L'^';
            case KEY_7:             return L'&';
            case KEY_8:             return L'*';
            case KEY_9:             return L'(';
            case KEY_0:             return L')';
            case KEY_MINUS:	        return L'_';
            case KEY_EQUAL:         return L'+';
            case KEY_Q:             return L'Q';
            case KEY_W:             return L'W';
            case KEY_E:             return L'E';
            case KEY_R:             return L'R';
            case KEY_T:             return L'T';
            case KEY_Y:             return L'Y';
            case KEY_U:             return L'U';
            case KEY_I:             return L'I';
            case KEY_O:             return L'O';
            case KEY_P:             return L'P';
            case KEY_LEFTBRACE:     return L'{';
            case KEY_RIGHTBRACE:    return L'}';
            case KEY_A:             return L'A';
            case KEY_S:             return L'S';
            case KEY_D:             return L'D';
            case KEY_F:             return L'F';
            case KEY_G:             return L'G';
            case KEY_H:             return L'H';
            case KEY_J:             return L'J';
            case KEY_K:             return L'K';
            case KEY_L:             return L'L';
            case KEY_SEMICOLON:     return L':';
            case KEY_APOSTROPHE:    return L'\"';
            case KEY_GRAVE:         return L'~';
            case KEY_BACKSLASH:     return L'|';
            case KEY_Z:             return L'Z';
            case KEY_X:             return L'X';
            case KEY_C:             return L'C';
            case KEY_V:             return L'V';
            case KEY_B:             return L'B';
            case KEY_N:             return L'N';
            case KEY_M:             return L'M';
            case KEY_COMMA:         return L'<';
            case KEY_DOT:           return L'>';
            case KEY_SLASH:         return L'?';
            case KEY_KPASTERISK:    return L'*';
            case KEY_SPACE:         return L' ';
            case KEY_KP7:           return L'7';
            case KEY_KP8:           return L'8';
            case KEY_KP9:           return L'9';
            case KEY_KPMINUS:       return L'-';
            case KEY_KP4:           return L'4';
            case KEY_KP5:           return L'5';
            case KEY_KP6:           return L'6';
            case KEY_KPPLUS:        return L'+';
            case KEY_KP1:           return L'1';
            case KEY_KP2:           return L'2';
            case KEY_KP3:           return L'3';
            case KEY_KP0:           return L'0';
            case KEY_KPDOT:         return L'.';
            case KEY_KPSLASH:       return L'/';
            case KEY_KPEQUAL:       return L'=';
            default:                return 0;
        }
    }
    else {
        switch (code) {
            case KEY_1:	            return L'1';
            case KEY_2:	            return L'2';
            case KEY_3:	            return L'3';
            case KEY_4:	            return L'4';
            case KEY_5:             return L'5';
            case KEY_6:             return L'6';
            case KEY_7:             return L'7';
            case KEY_8:             return L'8';
            case KEY_9:             return L'9';
            case KEY_0:             return L'0';
            case KEY_MINUS:	        return L'-';
            case KEY_EQUAL:         return L'=';
            case KEY_Q:             return L'q';
            case KEY_W:             return L'w';
            case KEY_E:             return L'e';
            case KEY_R:             return L'r';
            case KEY_T:             return L't';
            case KEY_Y:             return L'y';
            case KEY_U:             return L'u';
            case KEY_I:             return L'i';
            case KEY_O:             return L'o';
            case KEY_P:             return L'p';
            case KEY_LEFTBRACE:     return L'[';
            case KEY_RIGHTBRACE:    return L']';
            case KEY_A:             return L'a';
            case KEY_S:             return L's';
            case KEY_D:             return L'd';
            case KEY_F:             return L'f';
            case KEY_G:             return L'g';
            case KEY_H:             return L'h';
            case KEY_J:             return L'j';
            case KEY_K:             return L'k';
            case KEY_L:             return L'l';
            case KEY_SEMICOLON:     return L';';
            case KEY_APOSTROPHE:    return L'\'';
            case KEY_GRAVE:         return L'`';
            case KEY_BACKSLASH:     return L'\\';
            case KEY_Z:             return L'z';
            case KEY_X:             return L'x';
            case KEY_C:             return L'c';
            case KEY_V:             return L'v';
            case KEY_B:             return L'b';
            case KEY_N:             return L'n';
            case KEY_M:             return L'm';
            case KEY_COMMA:         return L',';
            case KEY_DOT:           return L'.';
            case KEY_SLASH:         return L'/';
            case KEY_KPASTERISK:    return L'*';
            case KEY_SPACE:         return L' ';
            case KEY_KP7:           return L'7';
            case KEY_KP8:           return L'8';
            case KEY_KP9:           return L'9';
            case KEY_KPMINUS:       return L'-';
            case KEY_KP4:           return L'4';
            case KEY_KP5:           return L'5';
            case KEY_KP6:           return L'6';
            case KEY_KPPLUS:        return L'+';
            case KEY_KP1:           return L'1';
            case KEY_KP2:           return L'2';
            case KEY_KP3:           return L'3';
            case KEY_KP0:           return L'0';
            case KEY_KPDOT:         return L'.';
            case KEY_KPSLASH:       return L'/';
            case KEY_KPEQUAL:       return L'=';
            default:                return 0;
        }
    }
}

} // namespace _priv
} // namespace Oryol


