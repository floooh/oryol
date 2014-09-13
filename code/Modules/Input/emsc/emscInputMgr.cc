//------------------------------------------------------------------------------
//  emscInputMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "emscInputMgr.h"
#include "Core/CoreFacade.h"

namespace Oryol {
namespace Input {
    
//------------------------------------------------------------------------------
emscInputMgr::emscInputMgr() {
    this->setupKeyTable();
    this->setupCallbacks();
    this->runLoopId = CoreFacade::Instance()->RunLoop()->Add([this]() { this->reset(); });
}

//------------------------------------------------------------------------------
emscInputMgr::~emscInputMgr() {
    this->discardCallbacks();
    CoreFacade::Instance()->RunLoop()->Remove(this->runLoopId);
}

//------------------------------------------------------------------------------
void
emscInputMgr::setCursorMode(CursorMode::Code mode) {
    if (CursorMode::Disabled == mode) {
        emscripten_request_pointerlock(0, true);
    }
    else {
        emscripten_exit_pointerlock();
    }
    // NOTE: base class setCursorMode will be called from the pointerlock event callback 
}

//------------------------------------------------------------------------------
void
emscInputMgr::reset() {
    if (this->keyboard.Attached()) {
        this->keyboard.reset();
    }
    if (this->mouse.Attached()) {
        this->mouse.reset();
    }
    if (this->touchpad.Attached()) {
        this->touchpad.reset();
    }
    for (int32 i = 0; i < MaxNumGamepads; i++) {
        if (this->gamepads[i].Attached()) {
            this->gamepads[i].reset();
        }
    }    
}

//------------------------------------------------------------------------------
void
emscInputMgr::setupCallbacks() {
    emscripten_set_keydown_callback(0, this, true, emscKeyDown);
    emscripten_set_keyup_callback(0, this, true, emscKeyUp);
    emscripten_set_keypress_callback(0, this, true, emscKeyPress);
    emscripten_set_mousedown_callback(0, this, true, emscMouseDown);
    emscripten_set_mouseup_callback(0, this, true, emscMouseUp);
    emscripten_set_mousemove_callback(0, this, true, emscMouseMove);
    emscripten_set_wheel_callback(0, this, true, emscWheel);
    emscripten_set_pointerlockchange_callback(0, this, true, emscPointerLockChange);
}

//------------------------------------------------------------------------------
void
emscInputMgr::discardCallbacks() {
    emscripten_set_keydown_callback(0, 0, true, 0);
    emscripten_set_keyup_callback(0, 0, true, 0);    
    emscripten_set_keypress_callback(0, 0, true, 0);
    emscripten_set_mousedown_callback(0, 0, true, 0);
    emscripten_set_mouseup_callback(0, 0, true, 0);
    emscripten_set_mousemove_callback(0, 0, true, 0);
    emscripten_set_wheel_callback(0, 0, true, 0);
    emscripten_set_pointerlockchange_callback(0, this, true, 0);
}

//------------------------------------------------------------------------------
EM_BOOL
emscInputMgr::emscKeyDown(int eventType, const EmscriptenKeyboardEvent* e, void* userData) {
    emscInputMgr* self = (emscInputMgr*) userData;
    o_assert_dbg(self);
    const Key::Code key = self->mapKey(e->keyCode);
    if (Key::InvalidKey != key) {
        if (!e->repeat) {
            self->keyboard.onKeyDown(key);
        }
        if (self->keyboard.IsCapturingText()) {
            // returning false enables keypress events, but also lets the
            // browser react to Tab, Backspace, etc... thus we need to 
            // filter these out
            if ((Key::Tab == key) || (Key::BackSpace == key) || (Key::Enter == key)) {
                // enable keypresses, but disable browser handling for Tab, BS, Enter
                return true;
            }
            else {
                // pass-thru event
                return false;
            }
        }
        else {
            // returning true disables keypress events, but also
            // suppresses the browser from handling Tab, Backspace...
            return true;
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
Mouse::Button
emscInputMgr::mapMouseButton(unsigned short html5Btn) const {
    switch (html5Btn) {
        case 0:  return Mouse::LMB;
        case 1:  return Mouse::MMB;
        case 2:  return Mouse::RMB;
        default: return Mouse::InvalidButton;
    }
}

//------------------------------------------------------------------------------
EM_BOOL
emscInputMgr::emscMouseDown(int eventType, const EmscriptenMouseEvent* e, void* userData) {
    emscInputMgr* self = (emscInputMgr*) userData;
    o_assert_dbg(self);
    const Mouse::Button btn = self->mapMouseButton(e->button);
    if (Mouse::InvalidButton != btn) {
        self->mouse.onButtonDown(btn);
    }
    return self->getCursorMode() == CursorMode::Disabled;
}

//------------------------------------------------------------------------------
EM_BOOL
emscInputMgr::emscMouseUp(int eventType, const EmscriptenMouseEvent* e, void* userData) {
    emscInputMgr* self = (emscInputMgr*) userData;
    o_assert_dbg(self);
    const Mouse::Button btn = self->mapMouseButton(e->button);
    if (Mouse::InvalidButton != btn) {
        self->mouse.onButtonUp(btn);
    }
    return self->getCursorMode() == CursorMode::Disabled;
}

//------------------------------------------------------------------------------
EM_BOOL
emscInputMgr::emscMouseMove(int eventType, const EmscriptenMouseEvent* e, void* userData) {
    emscInputMgr* self = (emscInputMgr*) userData;
    o_assert_dbg(self);

    // check if pointerlock is active, if yes directly obtain movement
    if (self->getCursorMode() == CursorMode::Disabled) {
        const glm::vec2 mov((float32)e->movementX, (float32)e->movementY);
        self->mouse.onMove(mov);
    }
    else {
        const glm::vec2 pos((float32)e->canvasX, (float32)e->canvasY);
        self->mouse.onPos(pos);        
    }
    return self->getCursorMode() == CursorMode::Disabled;    
}

//------------------------------------------------------------------------------
EM_BOOL
emscInputMgr::emscWheel(int eventType, const EmscriptenWheelEvent* e, void* userData) {
    emscInputMgr* self = (emscInputMgr*) userData;
    o_assert_dbg(self);
    const glm::vec2 scroll((float32)e->deltaX, (float32)e->deltaY);
    self->mouse.onScroll(scroll);
    return self->getCursorMode() == CursorMode::Disabled;
}

//------------------------------------------------------------------------------
EM_BOOL
emscInputMgr::emscPointerLockChange(int eventType, const EmscriptenPointerlockChangeEvent* e, void* userData) {
    emscInputMgr* self = (emscInputMgr*) userData;
    if (e->isActive) {
        ((inputMgrBase*)self)->setCursorMode(CursorMode::Disabled);
    }
    else {
        ((inputMgrBase*)self)->setCursorMode(CursorMode::Normal);
    }
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
    for (int32 i = 0; i < MaxNumKeys; i++) {
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

} // namespace Input
} // namespace Oryol