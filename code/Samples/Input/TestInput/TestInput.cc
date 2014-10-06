//------------------------------------------------------------------------------
//  TestInput.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "Dbg/Dbg.h"
#include "Input/Input.h"
#include "Core/String/StringConverter.h"

using namespace Oryol;

class TestInputApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
    
private:
    void testMouseButton(const Mouse& mouse, Mouse::Button btn, const char* name) const;
    void testKey(const Keyboard& keyboard, Key::Code key, const char* name) const;
    void printMouseState(const Mouse& mouse) const;
    void printKeyboardState(const Keyboard& keyboard) const;
    void printTouchpadState(const Touchpad& touchpad) const;
    glm::vec4 getClearColor(const Touchpad& touchpad) const;
    
    const glm::vec4 downColor{1.0f, 0.0f, 0.0f, 1.0f};
    const glm::vec4 upColor{0.0f, 0.0f, 1.0f, 1.0f};
    const glm::vec4 pressedColor{0.0f, 1.0f, 0.0f, 1.0f};
    const glm::vec4 defaultColor{1.0f, 1.0f, 1.0f, 0.5f};
};
OryolMain(TestInputApp);

//------------------------------------------------------------------------------
AppState::Code
TestInputApp::OnInit() {
    Gfx::Setup(GfxSetup::Window(800, 400, "Oryol Input Test Sample"));
    Dbg::Setup();
    Dbg::SetTextScale(glm::vec2(2.0f, 2.0f));
    Input::Setup();
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
void
TestInputApp::testMouseButton(const Mouse& mouse, Mouse::Button btn, const char* name) const {
    glm::vec4 color;
    if (mouse.ButtonDown(btn)) color = this->downColor;
    else if (mouse.ButtonUp(btn)) color = this->upColor;
    else if (mouse.ButtonPressed(btn)) color = this->pressedColor;
    else color = this->defaultColor;
    Dbg::TextColor(color);
    Dbg::PrintF(" %s", name);
}

//------------------------------------------------------------------------------
void
TestInputApp::testKey(const Keyboard& keyboard, Key::Code key, const char* name) const {
    glm::vec4 color;
    if (keyboard.KeyDown(key)) {
        Dbg::TextColor(this->downColor);
        Dbg::PrintF(" %s", Key::ToString(key));
    }
    else if (keyboard.KeyUp(key)) {
        Dbg::TextColor(this->upColor);
        Dbg::PrintF(" %s", Key::ToString(key));
    }
    else if (keyboard.KeyPressed(key)) {
        Dbg::TextColor(this->pressedColor);
        Dbg::PrintF(" %s", Key::ToString(key));
    }
}

//------------------------------------------------------------------------------
void
TestInputApp::printMouseState(const Mouse& mouse) const {
    if (mouse.Attached) {
        Dbg::TextColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        Dbg::Print("\n MOUSE STATUS (Enter for pointerlock):\n\n\r");
        
        this->testMouseButton(mouse, Mouse::LMB, "LMB");
        this->testMouseButton(mouse, Mouse::MMB, "MMB");
        this->testMouseButton(mouse, Mouse::RMB, "RMB");
        
        Dbg::TextColor(glm::vec4(1.0f));
        Dbg::PrintF("\n\r pos: %.3f %.3f\n\r mov: %.3f %.3f\n\r scroll: %.3f %.3f",
                    mouse.Position.x, mouse.Position.y,
                    mouse.Movement.x, mouse.Movement.y,
                    mouse.Scroll.x, mouse.Scroll.y);
    }
}

//------------------------------------------------------------------------------
void
TestInputApp::printKeyboardState(const Keyboard& keyboard) const {
    if (keyboard.Attached) {
        Dbg::TextColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
        Dbg::Print("\n\n\r KEYBOARD STATUS (Enter to capture text):\n\n\r");
        if (keyboard.KeyDown(Key::Enter)) {
            if (CursorMode::Disabled != Input::GetCursorMode()) {
                Input::SetCursorMode(CursorMode::Disabled);
            }
            else {
                Input::SetCursorMode(CursorMode::Normal);
            }
            if (!keyboard.IsCapturingText()) {
                Input::BeginCaptureText();
            }
            else {
                Input::EndCaptureText();
            }
        }
        if (keyboard.IsCapturingText()) {
            Dbg::Print(" capturing: ");
            String str = StringConverter::WideToUTF8(keyboard.CapturedText());
            Dbg::PrintF("%s\n\r", str.AsCStr());
        }
        else {
            Dbg::Print(" keys: ");
            for (int32 key = 0; key < Key::NumKeys; key++) {
                this->testKey(keyboard, (Key::Code)key, Key::ToString((Key::Code)key));
            }
        }
    }
}

//------------------------------------------------------------------------------
void
TestInputApp::printTouchpadState(const Touchpad& touchpad) const {
    if (touchpad.Attached) {
        Dbg::TextColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        Dbg::Print("\n\n\r TOUCHPAD STATUS:\n\n\r");
        if (touchpad.Tapped) {
            Dbg::TextColor(this->pressedColor);
        }
        else {
            Dbg::TextColor(this->defaultColor);
        }
        Dbg::Print(" TAPPED ");
        if (touchpad.DoubleTapped) {
            Dbg::TextColor(this->pressedColor);
        }
        else {
            Dbg::TextColor(this->defaultColor);
        }
        Dbg::Print("DOUBLETAPPED ");
        if (touchpad.Panning) {
            Dbg::TextColor(this->pressedColor);
        }
        else {
            Dbg::TextColor(this->defaultColor);
        }
        Dbg::Print("PANNING ");
        if (touchpad.Pinching) {
            Dbg::TextColor(this->pressedColor);
        }
        else {
            Dbg::TextColor(this->defaultColor);
        }
        Dbg::Print("PINCHING");
        Dbg::Print("\n\n\r");
        Dbg::TextColor(glm::vec4(1.0f));
        Dbg::PrintF(" touch position0: %.3f %.3f\n\r"
                    " touch movement0: %.3f %.3f\n\r"
                    " touch startPos0: %.3f %.3f\n\r"
                    " touch position1: %.3f %.3f\n\r"
                    " touch movement1: %.3f %.3f\n\r"
                    " touch startPos1: %.3f %.3f\n\r",
                    touchpad.Position(0).x, touchpad.Position(0).y,
                    touchpad.Movement(0).x, touchpad.Movement(0).y,
                    touchpad.StartPosition(0).x, touchpad.StartPosition(0).y,
                    touchpad.Position(1).x, touchpad.Position(1).y,
                    touchpad.Movement(1).x, touchpad.Movement(1).y,
                    touchpad.StartPosition(1).x, touchpad.StartPosition(1).y);
    }
}

//------------------------------------------------------------------------------
glm::vec4
TestInputApp::getClearColor(const Touchpad& touchpad) const {
    glm::vec4 clearColor(0.25f);
    if (touchpad.Tapped) {
        clearColor = this->downColor;
    }
    if (touchpad.DoubleTapped) {
        clearColor = this->upColor;
    }
    return clearColor;
}

//------------------------------------------------------------------------------
AppState::Code
TestInputApp::OnRunning() {

    // print input device status as debug text
    const Keyboard& keyboard = Input::Keyboard();
    const Mouse& mouse = Input::Mouse();
    const Touchpad& touchpad = Input::Touchpad();
    this->printMouseState(mouse);
    this->printKeyboardState(keyboard);
    this->printTouchpadState(touchpad);
    
    // draw frame
    Gfx::ApplyDefaultRenderTarget();
    Gfx::Clear(PixelChannel::RGBA, this->getClearColor(touchpad), 1.0f, 0);
    Dbg::DrawTextBuffer();
    Gfx::CommitFrame();
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
TestInputApp::OnCleanup() {
    Input::Discard();
    Dbg::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}
