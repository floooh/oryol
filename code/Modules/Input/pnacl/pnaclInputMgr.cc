//------------------------------------------------------------------------------
//  pnaclInputMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "pnaclInputMgr.h"
#include "Core/Core.h"
#include "Core/pnacl/pnaclInstance.h"
#include "Core/String/StringConverter.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
pnaclInputMgr::pnaclInputMgr() :
runLoopId(RunLoop::InvalidId) {
    // empty
}

//------------------------------------------------------------------------------
pnaclInputMgr::~pnaclInputMgr() {
    // empty
}

//------------------------------------------------------------------------------
void
pnaclInputMgr::setup(const InputSetup& setup) {
    inputMgrBase::setup(setup);
    this->Keyboard.Attached = true;
    this->Mouse.Attached = true;
    this->setupKeyTable();

    pnaclInstance::Instance()->enableInput([this] (const pp::InputEvent& e) {
        return this->handleEvent(e);
    });
    this->runLoopId = Core::PostRunLoop()->Add([this]() { this->reset(); });
}

//------------------------------------------------------------------------------
void
pnaclInputMgr::discard() {
    Core::PostRunLoop()->Remove(this->runLoopId);
    this->runLoopId = RunLoop::InvalidId;
    inputMgrBase::discard();
}

//------------------------------------------------------------------------------
bool
pnaclInputMgr::handleEvent(const pp::InputEvent& ie) {

    switch (ie.GetType())
    {
        case PP_INPUTEVENT_TYPE_MOUSEDOWN:
            return this->onMouseDown(pp::MouseInputEvent(ie));

        case PP_INPUTEVENT_TYPE_MOUSEUP:
            return this->onMouseUp(pp::MouseInputEvent(ie));

        case PP_INPUTEVENT_TYPE_MOUSEMOVE:
            return this->onMouseMove(pp::MouseInputEvent(ie));

        case PP_INPUTEVENT_TYPE_MOUSEENTER:
            return this->onMouseEnter(pp::MouseInputEvent(ie));

        case PP_INPUTEVENT_TYPE_MOUSELEAVE:
            return this->onMouseLeave(pp::MouseInputEvent(ie));

        case PP_INPUTEVENT_TYPE_WHEEL:
            return this->onWheel(pp::WheelInputEvent(ie));

        case PP_INPUTEVENT_TYPE_KEYDOWN:            
            return this->onKeyDown(pp::KeyboardInputEvent(ie));

        case PP_INPUTEVENT_TYPE_KEYUP:
            return this->onKeyUp(pp::KeyboardInputEvent(ie));

        case PP_INPUTEVENT_TYPE_CHAR:
            return this->onChar(pp::KeyboardInputEvent(ie));

        default:
            return false;
    }
}

//------------------------------------------------------------------------------
Mouse::Button
pnaclInputMgr::mapMouseButton(PP_InputEvent_MouseButton naclBtn) {
    switch (naclBtn) {
        case PP_INPUTEVENT_MOUSEBUTTON_LEFT:   return Mouse::LMB;
        case PP_INPUTEVENT_MOUSEBUTTON_MIDDLE: return Mouse::MMB;
        case PP_INPUTEVENT_MOUSEBUTTON_RIGHT:  return Mouse::RMB;
        default: return Mouse::InvalidButton;
    }    
}

//------------------------------------------------------------------------------
bool
pnaclInputMgr::onMouseDown(const pp::MouseInputEvent& ie) {
    Mouse::Button btn = this->mapMouseButton(ie.GetButton());
    if (Mouse::InvalidButton != btn) {
        this->Mouse.onButtonDown(btn);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
bool
pnaclInputMgr::onMouseUp(const pp::MouseInputEvent& ie) {
    Mouse::Button btn = this->mapMouseButton(ie.GetButton());
    if (Mouse::InvalidButton != btn) {
        this->Mouse.onButtonUp(btn);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
bool
pnaclInputMgr::onMouseMove(const pp::MouseInputEvent& ie) {
    pp::Point pos = ie.GetPosition();
    pp::Point mov = ie.GetMovement();
    this->Mouse.onPosMov(glm::vec2(pos.x(), pos.y()), glm::vec2(mov.x(), mov.y()));
    return true;
}

//------------------------------------------------------------------------------
bool
pnaclInputMgr::onMouseEnter(const pp::MouseInputEvent& ie) {
    Log::Info("FIXME: onMouseEnter()\n");
    return false;
}

//------------------------------------------------------------------------------
bool
pnaclInputMgr::onMouseLeave(const pp::MouseInputEvent& ie) {
    Log::Info("FIXME: onMouseLeave()\n");
    return false;
}

//------------------------------------------------------------------------------
bool
pnaclInputMgr::onWheel(const pp::WheelInputEvent& ie) {
    const pp::FloatPoint pos = ie.GetDelta();
    this->Mouse.onScroll(glm::vec2(pos.x(), pos.y()));
    return true;
}

//------------------------------------------------------------------------------
bool
pnaclInputMgr::onKeyDown(const pp::KeyboardInputEvent& ie) {
    const Key::Code key = this->mapKey(ie.GetKeyCode());
    if (Key::InvalidKey != key) {
        this->Keyboard.onKeyDown(key);
        return true;
    }
    Log::Info("unhandled key code: %d\n", ie.GetKeyCode());
    return false;
}

//------------------------------------------------------------------------------
bool
pnaclInputMgr::onKeyUp(const pp::KeyboardInputEvent& ie) {
    const Key::Code key = this->mapKey(ie.GetKeyCode());
    if (Key::InvalidKey != key) {
        this->Keyboard.onKeyUp(key);
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
bool
pnaclInputMgr::onChar(const pp::KeyboardInputEvent& ie) {
    std::string str = ie.GetCharacterText().AsString();
    const size_t len = str.length();
    if (len > 0) {
        static wchar_t wide[32] = { 0 };
        const unsigned char* src = (const unsigned char*) str.c_str();
        const int wlen = StringConverter::UTF8ToWide(src, len, wide, sizeof(wide));
        if (wlen > 0) {
            this->Keyboard.onChar(wide[0]);
        }
    }
    return false;
}

//------------------------------------------------------------------------------
Key::Code
pnaclInputMgr::mapKey(uint32_t naclKeyCode) const {
    if (naclKeyCode < MaxNumKeys) {
        return this->keyTable[naclKeyCode];
    }
    else {
        return Key::InvalidKey;
    }
}

//------------------------------------------------------------------------------
void
pnaclInputMgr::setupKeyTable() {
    
    // looks like the keycodes are identical with JS 
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
    this->keyTable[188] = Key::Comma;
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
