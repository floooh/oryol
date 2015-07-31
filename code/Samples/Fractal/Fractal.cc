//------------------------------------------------------------------------------
//  Fractal.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "Input/Input.h"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "shaders.h"

using namespace Oryol;

class FractalApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
    
private:
    ClearState clearState;
    Id fractalRenderTarget[2];
    Id fractalDrawState;
    Id displayDrawState;
    int32 frameIndex = 0;
    Shaders::Fractal::FractalFSParams fractalFSParams;
    Shaders::Display::DisplayFSParams displayFSParams;
};
OryolMain(FractalApp);

//------------------------------------------------------------------------------
AppState::Code
FractalApp::OnRunning() {

    this->frameIndex++;
    const int32 index0 = this->frameIndex % 2;
    const int32 index1 = (this->frameIndex + 1) % 2;

    // render fractal data
    Gfx::ApplyRenderTarget(this->fractalRenderTarget[index0], this->clearState);
    Gfx::ApplyDrawState(this->fractalDrawState);
    this->fractalFSParams.MousePos = Input::Mouse().Position / glm::vec2(800, 512);
    this->fractalFSParams.MousePos.y = 1.0f - this->fractalFSParams.MousePos.y;
    this->fractalFSParams.Texture = this->fractalRenderTarget[index1];
    Gfx::ApplyUniformBlock(this->fractalFSParams);
    Gfx::Draw(0);

    // render one frame
    Gfx::ApplyDefaultRenderTarget(this->clearState);
    Gfx::ApplyDrawState(this->displayDrawState);
    this->displayFSParams.Texture = this->fractalRenderTarget[index0];
    Gfx::ApplyUniformBlock(this->displayFSParams);
    Gfx::Draw(0);

    Gfx::CommitFrame();
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
FractalApp::OnInit() {
    Gfx::Setup(GfxSetup::Window(800, 512, "Fractal Sample"));
    Input::Setup();

    Id fsq = Gfx::CreateResource(MeshSetup::FullScreenQuad());

    auto fracRTSetup = TextureSetup::RenderTarget(800, 512);
    fracRTSetup.ColorFormat = PixelFormat::RGBA32F;
    fracRTSetup.DepthFormat = PixelFormat::None;
    fracRTSetup.MinFilter = TextureFilterMode::Nearest;
    fracRTSetup.MagFilter = TextureFilterMode::Nearest;
    this->fractalRenderTarget[0] = Gfx::CreateResource(fracRTSetup);
    this->fractalRenderTarget[1] = Gfx::CreateResource(fracRTSetup);
    Id fractShd = Gfx::CreateResource(Shaders::Fractal::CreateSetup());
    auto dss = DrawStateSetup::FromMeshAndShader(fsq, fractShd);
    dss.RasterizerState.CullFaceEnabled = false;
    dss.DepthStencilState.DepthCmpFunc = CompareFunc::Always;
    dss.BlendState.ColorFormat = fracRTSetup.ColorFormat;
    dss.BlendState.DepthFormat = fracRTSetup.DepthFormat;
    this->fractalDrawState = Gfx::CreateResource(dss);

    Id dispShd = Gfx::CreateResource(Shaders::Display::CreateSetup());
    dss = DrawStateSetup::FromMeshAndShader(fsq, dispShd);
    dss.RasterizerState.CullFaceEnabled = false;
    dss.DepthStencilState.DepthCmpFunc = CompareFunc::Always;
    this->displayDrawState = Gfx::CreateResource(dss);

    // clear both render targets to zero
    Gfx::ApplyRenderTarget(this->fractalRenderTarget[0], this->clearState);
    Gfx::ApplyRenderTarget(this->fractalRenderTarget[1], this->clearState);
    Gfx::CommitFrame();
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
FractalApp::OnCleanup() {
    Input::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}

