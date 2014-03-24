//------------------------------------------------------------------------------
//  Clear.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Application/App.h"
#include "Core/Module.h"
#include "IO/Module.h"
#include "Render/Module.h"

OryolApp("Clear", "1.0");

using namespace Oryol;
using namespace Oryol::Application;
using namespace Oryol::Render;
using namespace Oryol::Core;

AppState::Code OryolInit();
AppState::Code OryolRunning();
AppState::Code OryolCleanup();

//------------------------------------------------------------------------------
void
OryolMain() {
    App app;
    app.AddCallback(AppState::Init, &OryolInit);
    app.AddCallback(AppState::Running, &OryolRunning);
    app.AddCallback(AppState::Cleanup, &OryolCleanup);
    app.StartMainLoop();
}

//------------------------------------------------------------------------------
AppState::Code
OryolInit() {
    // setup modules
    Core::Module::Setup();
    IO::Module::Setup();
    Render::Module::Setup();
    
    // setup rendering system
    RenderFacade::Instance()->Setup(RenderSetup::Windowed(400, 300, "Oryol Clear Sample"));
    return AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
OryolRunning() {
    CoreFacade* coreFacade = CoreFacade::Instance();
    RenderFacade* renderFacade = RenderFacade::Instance();
    
    static float r = 0.0f, g = 0.0f, b = 0.0f;
    coreFacade->RunLoop()->Run();
    if (renderFacade->BeginFrame()) {
        renderFacade->ApplyState(Render::State::ClearColor, r, g, b, 1.0f);
        renderFacade->Clear(true, false, false);
        renderFacade->EndFrame();
        if ((r += 0.01f) > 1.0f) r = 0.0f;
        if ((g += 0.005f) > 1.0f) g = 0.0f;
        if ((b += 0.0025f) > 1.0f) b = 0.0f;
    }
    return renderFacade->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
OryolCleanup() {
    RenderFacade::Instance()->Discard();
    Render::Module::Discard();
    IO::Module::Discard();
    Core::Module::Discard();
    return AppState::Destroy;
}
