//------------------------------------------------------------------------------
//  TestInput.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/RenderFacade.h"
#include "Debug/Debug.h"
#include "Input/InputFacade.h"
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
    
    RenderFacade* render;
    InputFacade* input;
    
    const glm::vec4 downColor{1.0f, 0.0f, 0.0f, 1.0f};
    const glm::vec4 upColor{0.0f, 0.0f, 1.0f, 1.0f};
    const glm::vec4 pressedColor{0.0f, 1.0f, 0.0f, 1.0f};
    const glm::vec4 defaultColor{1.0f, 1.0f, 1.0f, 0.5f};
};
OryolMain(TestInputApp);

//------------------------------------------------------------------------------
AppState::Code
TestInputApp::OnInit() {
    this->render = RenderFacade::CreateSingle(RenderSetup::AsWindow(512, 256, false, "Oryol Input Test Sample"));
    Debug::Setup();
    this->input  = InputFacade::CreateSingle();
    
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
    Debug::TextColor(color);
    Debug::PrintF(" %s", name);
}

//------------------------------------------------------------------------------
void
TestInputApp::testKey(const Keyboard& keyboard, Key::Code key, const char* name) const {
    glm::vec4 color;
    if (keyboard.KeyDown(key)) {
        Debug::TextColor(this->downColor);
        Debug::PrintF(" %s", Key::ToString(key));
    }
    else if (keyboard.KeyUp(key)) {
        Debug::TextColor(this->upColor);
        Debug::PrintF(" %s", Key::ToString(key));
    }
    else if (keyboard.KeyPressed(key)) {
        Debug::TextColor(this->pressedColor);
        Debug::PrintF(" %s", Key::ToString(key));
    }
}

//------------------------------------------------------------------------------
AppState::Code
TestInputApp::OnRunning() {
    // render one frame
    if (this->render->BeginFrame()) {
    
        this->render->ApplyDefaultRenderTarget();
        this->render->Clear(PixelChannel::RGBA, glm::vec4(0.25f), 1.0f, 0);
        
        const Keyboard& keyboard = this->input->Keyboard();
        const Mouse& mouse = this->input->Mouse();
        
        // mouse status
        if (mouse.Attached()) {
            Debug::TextColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
            Debug::Print("\n MOUSE STATUS (Enter for pointerlock):\n\n\r");
        
            this->testMouseButton(mouse, Mouse::LMB, "LMB");
            this->testMouseButton(mouse, Mouse::MMB, "MMB");
            this->testMouseButton(mouse, Mouse::RMB, "RMB");
            
            Debug::TextColor(glm::vec4(1.0f));
            Debug::PrintF("\n\r pos: %.3f %.3f\n\r mov: %.3f %.3f\n\r scroll: %.3f %.3f",
                          mouse.Position().x, mouse.Position().y,
                          mouse.Movement().x, mouse.Movement().y,
                          mouse.Scroll().x, mouse.Scroll().y);
        }
        
        // keyboard status
        if (keyboard.Attached()) {
            Debug::TextColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
            Debug::Print("\n\n\r KEYBOARD STATUS (Enter to capture text):\n\n\r");
            if (keyboard.KeyDown(Key::Enter)) {
                if (CursorMode::Disabled != this->input->GetCursorMode()) {
                    this->input->SetCursorMode(CursorMode::Disabled);
                }
                else {
                    this->input->SetCursorMode(CursorMode::Normal);
                }
                if (!keyboard.IsCapturingText()) {
                    this->input->BeginCaptureText();
                }
                else {
                    this->input->EndCaptureText();
                }
            }
            if (keyboard.IsCapturingText()) {
                Debug::Print(" capturing: ");
                String str = StringConverter::WideToUTF8(keyboard.CapturedText());
                Debug::PrintF("%s\n\r", str.AsCStr());
            }
            else {
                Debug::Print(" keys: ");
                for (int32 key = 0; key < Key::NumKeys; key++) {
                    this->testKey(keyboard, (Key::Code)key, Key::ToString((Key::Code)key));
                }
            }
        }
        
        Debug::DrawTextBuffer();
        this->render->EndFrame();
    }
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
TestInputApp::OnCleanup() {
    this->input  = nullptr;
    this->render = nullptr;
    InputFacade::DestroySingle();
    Debug::Discard();
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
