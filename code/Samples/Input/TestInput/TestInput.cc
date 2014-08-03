//------------------------------------------------------------------------------
//  TestInput.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/RenderFacade.h"
#include "Debug/DebugFacade.h"
#include "Input/InputFacade.h"
#include "Core/String/StringConverter.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::Render;
using namespace Oryol::Debug;
using namespace Oryol::Input;

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
    DebugFacade* debug;
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
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(512, 256, "Oryol Input Test Sample"));
    this->debug  = DebugFacade::CreateSingle();
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
    this->debug->TextColor(color);
    this->debug->PrintF(" %s", name);
}

//------------------------------------------------------------------------------
void
TestInputApp::testKey(const Keyboard& keyboard, Key::Code key, const char* name) const {
    glm::vec4 color;
    if (keyboard.KeyDown(key)) {
        this->debug->TextColor(this->downColor);
        this->debug->PrintF(" %s", Key::ToString(key));
    }
    else if (keyboard.KeyUp(key)) {
        this->debug->TextColor(this->upColor);
        this->debug->PrintF(" %s", Key::ToString(key));
    }
    else if (keyboard.KeyPressed(key)) {
        this->debug->TextColor(this->pressedColor);
        this->debug->PrintF(" %s", Key::ToString(key));
    }
}

//------------------------------------------------------------------------------
AppState::Code
TestInputApp::OnRunning() {
    // render one frame
    if (this->render->BeginFrame()) {
    
        this->render->ApplyDefaultRenderTarget();
        this->render->Clear(Channel::RGBA, glm::vec4(0.25f), 1.0f, 0);
        
        const Keyboard& keyboard = this->input->Keyboard();
        const Mouse& mouse = this->input->Mouse();
        
        const glm::vec4 down(1.0f, 0.0f, 0.0f, 1.0f);
        const glm::vec4 up(0.0f, 0.0f, 1.0f, 1.0f);
        const glm::vec4 pressed(0.0f, 1.0f, 0.0f, 1.0f);
        const glm::vec4 def(1.0f, 1.0f, 1.0f, 0.5f);
        glm::vec4 color(1.0f, 1.0f, 1.0f, 0.5f);
        
        // mouse status
        if (mouse.Attached()) {
            this->debug->TextColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
            this->debug->Print("\n MOUSE STATUS:\n\n\r");
        
            this->testMouseButton(mouse, Mouse::LMB, "LMB");
            this->testMouseButton(mouse, Mouse::MMB, "MMB");
            this->testMouseButton(mouse, Mouse::RMB, "RMB");
            
            this->debug->TextColor(glm::vec4(1.0f));
            this->debug->PrintF("\n\r pos: %.3f %.3f\n\r mov: %.3f %.3f\n\r scroll: %.3f %.3f",
                                mouse.Position().x, mouse.Position().y,
                                mouse.Movement().x, mouse.Movement().y,
                                mouse.Scroll().x, mouse.Scroll().y);
        }
        
        // keyboard status
        if (keyboard.Attached()) {
            this->debug->TextColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
            this->debug->Print("\n\n\r KEYBOARD STATUS (press Enter to capture text):\n\n\r");
            if (keyboard.KeyDown(Key::Enter)) {
                if (!keyboard.IsCapturingText()) {
                    this->input->BeginCaptureText();
                }
                else {
                    this->input->EndCaptureText();
                }
            }
            if (keyboard.IsCapturingText()) {
                this->debug->Print(" capturing: ");
                String str = StringConverter::WideToUTF8(keyboard.CapturedText());
                this->debug->PrintF("%s\n\r", str.AsCStr());
            }
            else {
                this->debug->Print(" keys: ");
                for (int32 key = 0; key < Key::NumKeys; key++) {
                    this->testKey(keyboard, (Key::Code)key, Key::ToString((Key::Code)key));
                }
            }
        }
        
        this->debug->DrawTextBuffer();
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
    this->debug  = nullptr;
    InputFacade::DestroySingle();
    DebugFacade::DestroySingle();
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
