//------------------------------------------------------------------------------
//  Fractal.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "Input/Input.h"
#include "IMUI/IMUI.h"
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
    /// reset all fractal states to their default
    void reset();
    /// draw the ui
    void drawUI();
    /// update the fractal's area rect
    void updateFractalRect(float x0, float y0, float x1, float y1);
    /// re-create offscreen render-target if window size has changed (FIXME)
    void checkCreateRenderTargets();

    DisplayAttrs curDisplayAttrs;
    ClearState clearState;
    ResourceLabel offscreenRenderTargetLabel;
    Id offscreenRenderTarget[2];
    Id displayDrawState;
    int32 frameIndex = 0;
    bool clearFlag = true;
    bool dragStarted = false;
    ImVec2 dragStartPos;
    glm::vec4 fractalRect;
    struct {
        Id drawState;
        Shaders::Mandelbrot::VSParams vsParams;
        Shaders::Mandelbrot::FSParams fsParams;
    } mandelbrot;
    Shaders::Display::FSParams displayFSParams;
};
OryolMain(FractalApp);

//------------------------------------------------------------------------------
AppState::Code
FractalApp::OnRunning() {

    this->frameIndex++;
    const int32 index0 = this->frameIndex % 2;
    const int32 index1 = (this->frameIndex + 1) % 2;

    // re-create offscreen-rendertargets if needed
    this->checkCreateRenderTargets();

    // reset current fractal state if requested
    if (this->clearFlag) {
        this->clearFlag = false;
        Gfx::ApplyRenderTarget(this->offscreenRenderTarget[0], this->clearState);
        Gfx::ApplyRenderTarget(this->offscreenRenderTarget[1], this->clearState);
    }

    // render fractal data
    Gfx::ApplyRenderTarget(this->offscreenRenderTarget[index0], this->clearState);
    Gfx::ApplyDrawState(this->mandelbrot.drawState);
    this->mandelbrot.fsParams.Texture = this->offscreenRenderTarget[index1];
    Gfx::ApplyUniformBlock(this->mandelbrot.vsParams);
    Gfx::ApplyUniformBlock(this->mandelbrot.fsParams);
    Gfx::Draw(0);

    // render one frame
    Gfx::ApplyDefaultRenderTarget(this->clearState);
    Gfx::ApplyDrawState(this->displayDrawState);
    this->displayFSParams.Texture = this->offscreenRenderTarget[index0];
    Gfx::ApplyUniformBlock(this->displayFSParams);
    Gfx::Draw(0);

    this->drawUI();
    Gfx::CommitFrame();
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
FractalApp::OnInit() {
    Gfx::Setup(GfxSetup::Window(800, 512, "Fractal Sample"));
    Input::Setup();
    IMUI::Setup();

    // ImGui colors
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    const ImVec4 grey(1.0f, 1.0f, 1.0f, 0.5f);
    style.Colors[ImGuiCol_TitleBg] = style.Colors[ImGuiCol_TitleBgActive] = grey;
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.0f, 1.0f, 1.0f, 0.20f);
    style.Colors[ImGuiCol_Button] = grey;

    // a fullscreen quad mesh that's reused several times
    Id fsqFractal = Gfx::CreateResource(MeshSetup::FullScreenQuad(Gfx::QueryFeature(GfxFeature::OriginTopLeft)));
    Id fsqDisplay = Gfx::CreateResource(MeshSetup::FullScreenQuad(true));

    // draw state for rendering the final result to screen
    Id dispShd = Gfx::CreateResource(Shaders::Display::CreateSetup());
    auto dss = DrawStateSetup::FromMeshAndShader(fsqDisplay, dispShd);
    dss.RasterizerState.CullFaceEnabled = false;
    dss.DepthStencilState.DepthCmpFunc = CompareFunc::Always;
    this->displayDrawState = Gfx::CreateResource(dss);

    // setup 2 ping-poing fp32 render targets which hold the current fractal state
    this->checkCreateRenderTargets();

    // setup mandelbrot state
    dss = DrawStateSetup::FromMeshAndShader(fsqFractal, Gfx::CreateResource(Shaders::Mandelbrot::CreateSetup()));
    dss.RasterizerState.CullFaceEnabled = false;
    dss.DepthStencilState.DepthCmpFunc = CompareFunc::Always;
    dss.BlendState.ColorFormat = PixelFormat::RGBA32F;
    dss.BlendState.DepthFormat = PixelFormat::None;
    this->mandelbrot.drawState = Gfx::CreateResource(dss);

    // initialize fractal states
    this->reset();

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
FractalApp::OnCleanup() {
    IMUI::Discard();
    Input::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
void
FractalApp::drawUI() {
    const DisplayAttrs& dispAttrs = Gfx::DisplayAttrs();
    const float fbWidth = (float) dispAttrs.FramebufferWidth;
    const float fbHeight = (float) dispAttrs.FramebufferHeight;

    IMUI::NewFrame();

    // draw the controls window
    ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiSetCond_Once);
    ImGui::Begin("Controls", nullptr, ImVec2(250, 110));
    ImGui::Text("mouse-drag a rectangle\nto zoom in");
    this->clearFlag = ImGui::Button("Redraw"); ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        this->clearFlag = true;
        this->reset();
    }
    ImGui::SliderFloat("Colors", &this->displayFSParams.NumColors, 2.0f, 256.0f);

    // handle dragging
    if (!ImGui::IsMouseHoveringAnyWindow() && ImGui::IsMouseClicked(0)) {
        this->dragStarted = true;
        this->dragStartPos = ImGui::GetMousePos();
    }
    if (this->dragStarted) {
        const ImVec2& mousePos = ImGui::GetMousePos();
        if (ImGui::IsMouseReleased(0)) {
            this->dragStarted = false;
            this->updateFractalRect(this->dragStartPos.x, this->dragStartPos.y, mousePos.x, mousePos.y);
            this->clearFlag = true;
        }
        else {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            drawList->PushClipRect(ImVec4(0, 0, fbWidth, fbHeight));
            drawList->AddRect(this->dragStartPos, ImGui::GetMousePos(), 0xFFFFFFFF);
            drawList->PopClipRect();
        }
    }

    ImGui::End();
    ImGui::Render();
}

//------------------------------------------------------------------------------
void
FractalApp::reset() {
    this->fractalRect = glm::vec4(-2.0, -2.0, 2.0, 2.0);   // x0,y0,x1,y1
    this->displayFSParams.NumColors = 8.0f;
    this->mandelbrot.vsParams.Rect = this->fractalRect;
}

//------------------------------------------------------------------------------
void
FractalApp::updateFractalRect(float x0, float y0, float x1, float y1) {

    if ((x0 == x1) || (y0 == y1)) return;
    if (x0 > x1) std::swap(x0, x1);
    if (y0 > y1) std::swap(y0, y1);

    const DisplayAttrs& attrs = Gfx::DisplayAttrs();
    const float fbWidth = (float) attrs.FramebufferWidth;
    const float fbHeight = (float) attrs.FramebufferHeight;
    glm::vec4 rel = glm::vec4(x0, y0, x1, y1) / glm::vec4(fbWidth, fbHeight, fbWidth, fbHeight);
    const float curX0 = this->fractalRect.x;
    const float curY0 = this->fractalRect.y;
    const float curWidth = this->fractalRect.z - curX0;
    const float curHeight = this->fractalRect.w - curY0;

    this->fractalRect = glm::vec4(
        curX0 + (curWidth * rel.x),
        curY0 + (curHeight * rel.y),
        curX0 + (curWidth * rel.z),
        curY0 + (curHeight * rel.w));
    this->mandelbrot.vsParams.Rect = this->fractalRect;
}

//------------------------------------------------------------------------------
void
FractalApp::checkCreateRenderTargets() {
    // this checks whether the window size has changed, and if yes,
    // re-creates the offscreen-rendertargets
    // FIXME: this should normally be handled by Oryol using
    // relative-sized render-targets, alas, this feature is not yet implemented
    const DisplayAttrs& attrs = Gfx::DisplayAttrs();
    if ((attrs.FramebufferWidth != this->curDisplayAttrs.FramebufferWidth) ||
        (attrs.FramebufferHeight != this->curDisplayAttrs.FramebufferHeight)) {

        // window size has changed, re-create render targets
        Log::Info("(re-)create render targets\n");
        this->curDisplayAttrs = attrs;

        // first destroy previous render targets
        if (ResourceLabel::Invalid != this->offscreenRenderTargetLabel) {
            Gfx::DestroyResources(this->offscreenRenderTargetLabel);
        }
        this->offscreenRenderTargetLabel = Gfx::PushResourceLabel();
        auto offscreenRTSetup = TextureSetup::RenderTarget(attrs.FramebufferWidth, attrs.FramebufferHeight);
        offscreenRTSetup.ColorFormat = PixelFormat::RGBA32F;
        offscreenRTSetup.DepthFormat = PixelFormat::None;
        offscreenRTSetup.MinFilter = TextureFilterMode::Nearest;
        offscreenRTSetup.MagFilter = TextureFilterMode::Nearest;
        this->offscreenRenderTarget[0] = Gfx::CreateResource(offscreenRTSetup);
        this->offscreenRenderTarget[1] = Gfx::CreateResource(offscreenRTSetup);
        Gfx::PopResourceLabel();
        this->clearFlag = true;
    }
}
