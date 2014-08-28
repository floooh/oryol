//------------------------------------------------------------------------------
//  NanoVG.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Render/RenderFacade.h"
#include "Input/InputFacade.h"
#include "Time/Clock.h"
#include "Render/gl/gl_impl.h"
#include "nanovg/nanovg.h"
#if ORYOL_OPENGLES2
#define NANOVG_GLES2_IMPLEMENTATION
#elif ORYOL_OSX
#define NANOVG_GL3_IMPLEMENTATION
#else 
#define NANOVG_GL2_IMPLEMENTATION
#endif
#include "nanovg/nanovg_gl.h"
#include "demo.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::Render;
using namespace Oryol::Time;
using namespace Oryol::Input;

// derived application class
class NanoVGApp : public App {
public:
    virtual AppState::Code OnInit();
    virtual AppState::Code OnRunning();
    virtual AppState::Code OnCleanup();
    
private:
    RenderFacade* render = nullptr;
    InputFacade* input = nullptr;
    NVGcontext* vg = nullptr;
    DemoData data;
};
OryolMain(NanoVGApp);

//------------------------------------------------------------------------------
AppState::Code
NanoVGApp::OnRunning() {

    const DisplayAttrs& attrs = this->render->GetDisplayAttrs();
    const int32 w = attrs.FramebufferWidth;
    const int32 h = attrs.FramebufferHeight;
    const float32 aspect = float32(w) / float32(h);
    if (this->render->BeginFrame()) {
        this->render->ApplyDefaultRenderTarget();
        this->render->Clear(Channel::All, glm::vec4(0.3f), 1.0f, 0);

        const Mouse& mouse = this->input->Mouse();
        const int32 mouseX = mouse.Position().x;
        const int32 mouseY = mouse.Position().y;
        const int32 blowup = 0;
        const float64 t = Clock::Now().Since(0).AsSeconds();
        
        nvgBeginFrame(this->vg, w, h, aspect);
        renderDemo(this->vg, mouseX, mouseY, w, h, t, blowup, &this->data);
        nvgEndFrame(this->vg);
        
        this->render->EndFrame();
    }
    
    // continue running or quit?
    return render->QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
NanoVGApp::OnInit() {
    this->render = RenderFacade::CreateSingle(RenderSetup::Windowed(1024, 600, "Oryol NanoVG Sample", PixelFormat::RGBA8, PixelFormat::D24S8));
    this->input = InputFacade::CreateSingle();
    
    #if ORYOL_OPENGLES2
    this->vg = nvgCreateGLES2(NVG_STENCIL_STROKES | NVG_ANTIALIAS);
    #elif ORYOL_OSX
    this->vg = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_ANTIALIAS);
    #else 
    this->vg = nvgCreateGL2(NVG_STENCIL_STROKES | NVG_ANTIALIAS);
    #endif
    loadDemoData(this->vg, &this->data);
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
NanoVGApp::OnCleanup() {
    // cleanup everything
    freeDemoData(this->vg, &this->data);
    #if ORYOL_OPENGLES2
    nvgDeleteGLES2(this->vg);
    #elif ORYOL_OSX
    nvgDeleteGL3(this->vg);
    #else
    nvgCreateGL2(this->vg);
    #endif
    
    this->input = nullptr;
    this->render = nullptr;
    InputFacade::DestroySingle();
    RenderFacade::DestroySingle();
    return App::OnCleanup();
}
