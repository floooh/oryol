//------------------------------------------------------------------------------
//  DebugText.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/RenderFacade.h"
#include "Debug/DebugFacade.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::Render;
using namespace Oryol::Debug;

// derived application class
class DebugTextApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    RenderFacade* render;
    DebugFacade* debug;
};
OryolMain(DebugTextApp);

//------------------------------------------------------------------------------
AppState::Code
DebugTextApp::OnInit() {
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(1024, 768, "Oryol DebugText Sample"));
    this->debug  = DebugFacade::CreateSingle();
    this->debug->SetTextColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    this->debug->SetTextScale(glm::vec2(1.0f, 2.0f));
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
DebugTextApp::OnRunning() {
    // we can do text rendering everywhere in the frame...
    this->debug->Text("Hello World!\n\rBlarghl?!?");
    
    // render one frame
    if (this->render->BeginFrame()) {

        // clear, apply mesh and shader program, and draw
        this->render->ApplyState(Render::State::ClearColor, 0.5f, 0.5f, 0.5f, 0.0f);
        this->render->Clear(true, false, false);
        
        // the text buffer must be rendered within BeginFrame()/EndFrame() though...
        this->debug->DrawTextBuffer();
        
        this->render->EndFrame();
    }
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
DebugTextApp::OnCleanup() {
    this->render = nullptr;
    this->debug  = nullptr;
    DebugFacade::DestroySingle();
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
