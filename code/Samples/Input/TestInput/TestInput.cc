//------------------------------------------------------------------------------
//  TestInput.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/Render.h"
#include "Dbg/Dbg.h"
#include "Input/Input.h"
#include "Core/String/StringConverter.h"

using namespace Oryol;

// derived application class
class TestInputApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    void testMouseButton(const Mouse& mouse, Mouse::Button btn, const char* name) const;
    void testKey(const Keyboard& keyboard, Key::Code key, const char* name) const;
    
    const glm::vec4 downColor{1.0f, 0.0f, 0.0f, 1.0f};
    const glm::vec4 upColor{0.0f, 0.0f, 1.0f, 1.0f};
    const glm::vec4 pressedColor{0.0f, 1.0f, 0.0f, 1.0f};
    const glm::vec4 defaultColor{1.0f, 1.0f, 1.0f, 0.5f};
};
OryolMain(TestInputApp);

//------------------------------------------------------------------------------
AppState::Code
TestInputApp::OnInit() {
    Render::Setup(RenderSetup::AsWindow(512, 256, false, "Oryol Input Test Sample"));
    Dbg::Setup();
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
AppState::Code
TestInputApp::OnRunning() {
    // render one frame
    if (Render::BeginFrame()) {
    
        Render::ApplyDefaultRenderTarget();
        Render::Clear(PixelChannel::RGBA, glm::vec4(0.25f), 1.0f, 0);
        
        const Keyboard& keyboard = Input::Keyboard();
        const Mouse& mouse = Input::Mouse();
        
        // mouse status
        if (mouse.Attached()) {
            Dbg::TextColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
            Dbg::Print("\n MOUSE STATUS (Enter for pointerlock):\n\n\r");
        
            this->testMouseButton(mouse, Mouse::LMB, "LMB");
            this->testMouseButton(mouse, Mouse::MMB, "MMB");
            this->testMouseButton(mouse, Mouse::RMB, "RMB");
            
            Dbg::TextColor(glm::vec4(1.0f));
            Dbg::PrintF("\n\r pos: %.3f %.3f\n\r mov: %.3f %.3f\n\r scroll: %.3f %.3f",
                        mouse.Position().x, mouse.Position().y,
                        mouse.Movement().x, mouse.Movement().y,
                        mouse.Scroll().x, mouse.Scroll().y);
        }
        
        // keyboard status
        if (keyboard.Attached()) {
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
        
        Dbg::DrawTextBuffer();
        Render::EndFrame();
    }
    
    // continue running or quit?
    return Render::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
TestInputApp::OnCleanup() {
    Input::Discard();
    Dbg::Discard();
    Render::Discard();
    return App::OnCleanup();
}
