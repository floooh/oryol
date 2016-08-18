//------------------------------------------------------------------------------
//  uwpInputMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "uwpInputMgr.h"
#include "Core/Core.h"

// FIXME
#include "Core/String/StringConverter.h"

using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Devices::Input;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;

namespace Oryol {
namespace _priv {

uwpInputMgr* uwpInputMgr::ptr = nullptr;
static Key::Code keymap[256];

//------------------------------------------------------------------------------
// helper C++/CX class with all the UWP input event handlers
ref class uwpEventBridge sealed {
private:
    MouseButton::Code mapButton(PointerPointProperties^ props) {
        if (props->IsLeftButtonPressed) return MouseButton::Left;
        else if (props->IsRightButtonPressed) return MouseButton::Right;
        else if (props->IsMiddleButtonPressed) return MouseButton::Middle;
        else return MouseButton::InvalidMouseButton;
    }
public:
    void onKeyDown(CoreWindow^ sender, KeyEventArgs^ args) {
        if (uwpInputMgr::ptr) {
            uwpInputMgr::ptr->keyboard.onKeyDown(uwpInputMgr::mapKey(args));
        }
    }
    void onKeyUp(CoreWindow^ sender, KeyEventArgs^ args) {
        if (uwpInputMgr::ptr) {
            uwpInputMgr::ptr->keyboard.onKeyUp(uwpInputMgr::mapKey(args));
        }
    }
    void onChar(CoreWindow^ sender, CharacterReceivedEventArgs^ args) {
        if (uwpInputMgr::ptr) {
            uwpInputMgr::ptr->keyboard.onChar(args->KeyCode);
        }
    }
    void onPointerPressed(CoreWindow^ sender, PointerEventArgs^ args) {
        if (uwpInputMgr::ptr) {
            MouseButton::Code btn = this->mapButton(args->CurrentPoint->Properties);
            if (MouseButton::InvalidMouseButton != btn) {
                PointerLockMode::Code lockMode = uwpInputMgr::ptr->mouse.onButtonDown(btn);
                uwpInputMgr::ptr->onPointerLock(lockMode);
            }
        }
    }
    void onPointerReleased(CoreWindow^ sender, PointerEventArgs^ args) {
        if (uwpInputMgr::ptr) {
            // we don't get the information *which* button is released,
            // it's the last one pressed (WTF!), see here for the whole
            // clusterfuck: https://msdn.microsoft.com/en-us/library/windows/apps/windows.ui.xaml.uielement.pointerreleased
            // so what we'll do is simply release all mouse buttons that
            // are pressed, this should be okay-ish, we simply don't
            // allow multiple mouse buttons pressed on UWP
            for (int i = 0; i < MouseButton::NumMouseButtons; i++) {
                if (uwpInputMgr::ptr->mouse.buttonPressed((MouseButton::Code)i)) {
                    PointerLockMode::Code lockMode = uwpInputMgr::ptr->mouse.onButtonUp((MouseButton::Code)i);
                    uwpInputMgr::ptr->onPointerLock(lockMode);
                }
            }
        }
    }
    void onPointerMoved(CoreWindow^ sender, PointerEventArgs^ args) {
        if (uwpInputMgr::ptr) {
            glm::vec2 pos(args->CurrentPoint->Position.X, args->CurrentPoint->Position.Y);
            uwpInputMgr::ptr->mouse.onPosMov(pos);
        }
    }
    void onWheel(CoreWindow^ sender, PointerEventArgs^ args) {
        if (uwpInputMgr::ptr) {
            int wheelDelta = args->CurrentPoint->Properties->MouseWheelDelta;
            uwpInputMgr::ptr->mouse.onScroll(glm::vec2(0.0f, float(wheelDelta)));
        }
    }
};

static uwpEventBridge^ uwpEventBridgePtr = nullptr;

//------------------------------------------------------------------------------
uwpInputMgr::uwpInputMgr() :
runLoopId(RunLoop::InvalidId) {
    o_assert(nullptr == ptr);
    ptr = this;
    uwpEventBridgePtr = ref new uwpEventBridge();
    setupKeymap();
}

//------------------------------------------------------------------------------
uwpInputMgr::~uwpInputMgr() {
    o_assert(nullptr != ptr);
    ptr = nullptr;
}

//------------------------------------------------------------------------------
void
uwpInputMgr::setup(const InputSetup& setup) {
    inputMgrBase::setup(setup);
    
    // check if keyboard, mouse and touchpad are present
    KeyboardCapabilities^ kbdCaps = ref new KeyboardCapabilities();
    this->keyboard.attached = kbdCaps->KeyboardPresent != 0;
    MouseCapabilities^ mouseCaps = ref new MouseCapabilities();
    this->mouse.attached = mouseCaps->MousePresent != 0;
    // FIXME: ignore multi-touch, I don't have a device to test on
    this->touchpad.attached = false;

    // register keyboard event handlers
    CoreWindow^ win = CoreWindow::GetForCurrentThread();
    if (this->keyboard.attached) {
        win->KeyDown += 
            ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(uwpEventBridgePtr, &uwpEventBridge::onKeyDown);
        win->KeyUp +=
            ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(uwpEventBridgePtr, &uwpEventBridge::onKeyUp);
        win->CharacterReceived +=
            ref new TypedEventHandler<CoreWindow^, CharacterReceivedEventArgs^>(uwpEventBridgePtr, &uwpEventBridge::onChar);
    }

    // register mouse even handlers
    if (this->mouse.attached) {
        win->PointerPressed +=
            ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(uwpEventBridgePtr, &uwpEventBridge::onPointerPressed);
        win->PointerReleased +=
            ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(uwpEventBridgePtr, &uwpEventBridge::onPointerReleased);
        win->PointerMoved +=
            ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(uwpEventBridgePtr, &uwpEventBridge::onPointerMoved);
        win->PointerWheelChanged +=
            ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(uwpEventBridgePtr, &uwpEventBridge::onWheel);
    }

    // attach our reset callback to the global runloop
    this->runLoopId = Core::PostRunLoop()->Add([this]() { this->reset(); });
}

//------------------------------------------------------------------------------
void
uwpInputMgr::discard() {
    Core::PostRunLoop()->Remove(this->runLoopId);
    this->runLoopId = RunLoop::InvalidId;
    inputMgrBase::discard();
}

//------------------------------------------------------------------------------
void
uwpInputMgr::onPointerLock(PointerLockMode::Code lockMode) {
    if (PointerLockMode::Enable == lockMode) {
        Log::Info("FIXME: make mouse invisible\n");    
    }
    else if (PointerLockMode::Disable == lockMode) {
        Log::Info("FIXME: make mouse visible\n");
    }
}

//------------------------------------------------------------------------------
Key::Code
uwpInputMgr::mapKey(KeyEventArgs^ args) {
    uint8_t vk = (uint8_t)args->VirtualKey;
    return keymap[vk];
}

//------------------------------------------------------------------------------
void 
uwpInputMgr::setupKeymap() {
    for (int i = 0; i < 256; i++) {
        keymap[i] = Key::InvalidKey;
    }
    keymap[0x20] = Key::Space;
    keymap[0xBC] = Key::Comma;
    keymap[0xBD] = Key::Minus;
    keymap[0xBE] = Key::Period;
    keymap[0xBF] = Key::Slash;
    keymap[0x30] = Key::N0;
    keymap[0x31] = Key::N1;
    keymap[0x32] = Key::N2;
    keymap[0x33] = Key::N3;
    keymap[0x34] = Key::N4;
    keymap[0x35] = Key::N5;
    keymap[0x36] = Key::N6;
    keymap[0x37] = Key::N7;
    keymap[0x38] = Key::N8;
    keymap[0x39] = Key::N9;
    keymap[0xBA] = Key::Semicolon;
    keymap[0xBB] = Key::Equal;
    keymap[0x41] = Key::A;
    keymap[0x42] = Key::B;
    keymap[0x43] = Key::C;
    keymap[0x44] = Key::D;
    keymap[0x45] = Key::E;
    keymap[0x46] = Key::F;
    keymap[0x47] = Key::G;
    keymap[0x48] = Key::H;
    keymap[0x49] = Key::I;
    keymap[0x4A] = Key::J;
    keymap[0x4B] = Key::K;
    keymap[0x4C] = Key::L;
    keymap[0x4D] = Key::M;
    keymap[0x4E] = Key::N;
    keymap[0x4F] = Key::O;
    keymap[0x50] = Key::P;
    keymap[0x51] = Key::Q;
    keymap[0x52] = Key::R;
    keymap[0x53] = Key::S;
    keymap[0x54] = Key::T;
    keymap[0x55] = Key::U;
    keymap[0x56] = Key::V;
    keymap[0x57] = Key::W;
    keymap[0x58] = Key::X;
    keymap[0x59] = Key::Y;
    keymap[0x5A] = Key::Z;
    keymap[0xDB] = Key::LeftBracket;
    keymap[0xDC] = Key::BackSlash;
    keymap[0xDD] = Key::RightBracket;
    keymap[0xDE] = Key::GraveAccent;
    keymap[0x1B] = Key::Escape;
    keymap[0x0D] = Key::Enter;
    keymap[0x09] = Key::Tab;
    keymap[0x08] = Key::BackSpace;
    keymap[0x2D] = Key::Insert;
    keymap[0x2E] = Key::Delete;
    keymap[0x27] = Key::Right;
    keymap[0x25] = Key::Left;
    keymap[0x28] = Key::Down;
    keymap[0x26] = Key::Up;
    keymap[0x21] = Key::PageUp;
    keymap[0x22] = Key::PageDown;
    keymap[0x24] = Key::Home;
    keymap[0x23] = Key::End;
    keymap[0x14] = Key::CapsLock;
    keymap[0x91] = Key::ScrollLock;
    keymap[0x90] = Key::NumLock;
    keymap[0x2A] = Key::PrintScreen;
    keymap[0x13] = Key::Pause;
    keymap[0x70] = Key::F1;
    keymap[0x71] = Key::F2;
    keymap[0x72] = Key::F3;
    keymap[0x73] = Key::F4;
    keymap[0x74] = Key::F5;
    keymap[0x75] = Key::F6;
    keymap[0x76] = Key::F7;
    keymap[0x77] = Key::F8;
    keymap[0x78] = Key::F9;
    keymap[0x79] = Key::F10;
    keymap[0x7A] = Key::F11;
    keymap[0x7B] = Key::F12;
    keymap[0x7C] = Key::F13;
    keymap[0x7D] = Key::F14;
    keymap[0x7E] = Key::F15;
    keymap[0x7F] = Key::F16;
    keymap[0x80] = Key::F17;
    keymap[0x81] = Key::F18;
    keymap[0x82] = Key::F19;
    keymap[0x83] = Key::F20;
    keymap[0x84] = Key::F21;
    keymap[0x85] = Key::F22;
    keymap[0x86] = Key::F23;
    keymap[0x87] = Key::F24;
    keymap[0x60] = Key::Num0;
    keymap[0x61] = Key::Num1;
    keymap[0x62] = Key::Num2;
    keymap[0x63] = Key::Num3;
    keymap[0x64] = Key::Num4;
    keymap[0x65] = Key::Num5;
    keymap[0x66] = Key::Num6;
    keymap[0x67] = Key::Num7;
    keymap[0x68] = Key::Num8;
    keymap[0x69] = Key::Num9;
    keymap[0x6A] = Key::NumMultiply;
    keymap[0x6B] = Key::NumAdd;
    keymap[0x6C] = Key::NumEqual;
    keymap[0x6D] = Key::NumSubtract;
    keymap[0x6E] = Key::NumDecimal;
    keymap[0x6F] = Key::NumDivide;
    keymap[0xA0] = Key::LeftShift;
    keymap[0xA2] = Key::LeftControl;
    keymap[0xA4] = Key::LeftAlt;
    keymap[0x5B] = Key::LeftSuper;
    keymap[0xA1] = Key::RightShift;
    keymap[0xA3] = Key::RightControl;
    keymap[0xA5] = Key::RightAlt;
    keymap[0x5C] = Key::RightSuper;
    keymap[0x10] = Key::LeftShift;      // no left/right shift on UWP
    keymap[0x11] = Key::LeftControl;    // no left/right control on UWP
    keymap[0x12] = Key::LeftAlt;        // no left/right alt on UWP
    keymap[0xC0] = Key::Apostrophe;     // tilde key
}

} // namespace _priv
} // namespace Oryol
