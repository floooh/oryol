//------------------------------------------------------------------------------
//  Fractal.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
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
    enum Type : int {
        Mandelbrot = 0,
        Julia,
        NumTypes
    };

    /// reset all fractal states to their default
    void reset();
    /// zoom-out to initial rectangle
    void zoomOut();
    /// draw the ui
    void drawUI();
    /// set current bounds rectangle (either Mandelbrot or Julia context)
    void setBounds(Type type, const glm::vec4& bounds);
    /// get current bounds (either Mandelbrot or Julia context)
    glm::vec4 getBounds(Type type) const;
    /// convert mouse pos to fractal coordinate system pos
    glm::vec2 convertPos(float x, float y, const glm::vec4& bounds) const;
    /// update the fractal's area rect
    void updateFractalRect(float x0, float y0, float x1, float y1);
    /// update the Julia set position from a mouse position
    void updateJuliaPos(float x, float y);
    /// re-create offscreen render-target if window size has changed (FIXME)
    void recreateRenderTargets(const DisplayAttrs& attrs);

    ClearState fractalClearState = ClearState::ClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
    ClearState noClearState = ClearState::ClearNone();
    ResourceLabel offscreenRenderTargetLabel;
    Id offscreenRenderTarget[2];
    DrawState dispDrawState;
    DisplayShader::FSParams dispFSParams;
    int frameIndex = 0;
    bool clearFlag = true;
    bool dragStarted = false;
    ImVec2 dragStartPos;
    Type fractalType = Mandelbrot;
    struct {
        DrawState drawState;
        Mandelbrot::VSParams vsParams;
    } mandelbrot;
    struct {
        DrawState drawState;
        Julia::VSParams vsParams;
        Julia::FSParams fsParams;
    } julia;
};
OryolMain(FractalApp);

//------------------------------------------------------------------------------
AppState::Code
FractalApp::OnRunning() {

    // reset current fractal state if requested
    if (this->clearFlag) {
        this->clearFlag = false;
        Gfx::ApplyRenderTarget(this->offscreenRenderTarget[0], this->fractalClearState);
        Gfx::ApplyRenderTarget(this->offscreenRenderTarget[1], this->fractalClearState);
    }

    this->frameIndex++;
    const int index0 = this->frameIndex % 2;
    const int index1 = (this->frameIndex + 1) % 2;
    const Id& curTarget = this->offscreenRenderTarget[index0];
    const Id& curTexture = this->offscreenRenderTarget[index1];

    // render next fractal iteration
    Gfx::ApplyRenderTarget(curTarget, this->noClearState);
    if (Mandelbrot == this->fractalType) {
        this->mandelbrot.drawState.FSTexture[Textures::Texture] = curTexture;
        Gfx::ApplyDrawState(this->mandelbrot.drawState);
        Gfx::ApplyUniformBlock(this->mandelbrot.vsParams);
    }
    else {
        this->julia.drawState.FSTexture[Textures::Texture] = curTexture;
        Gfx::ApplyDrawState(this->julia.drawState);
        Gfx::ApplyUniformBlock(this->julia.vsParams);
        Gfx::ApplyUniformBlock(this->julia.fsParams);
    }
    Gfx::Draw(0);

    // map fractal state to display
    Gfx::ApplyDefaultRenderTarget(this->noClearState);
    this->dispDrawState.FSTexture[Textures::Texture] = curTarget;
    Gfx::ApplyDrawState(this->dispDrawState);
    Gfx::ApplyUniformBlock(this->dispFSParams);
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
    Gfx::Subscribe([this](const GfxEvent& event) {
        if (event.Type == GfxEvent::DisplayModified) {
            this->recreateRenderTargets(event.DisplayAttrs);
        }
    });

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
    auto fsqSetup = MeshSetup::FullScreenQuad(Gfx::QueryFeature(GfxFeature::OriginTopLeft));
    Id fsq = Gfx::CreateResource(fsqSetup);
    this->mandelbrot.drawState.Mesh[0] = fsq;
    this->julia.drawState.Mesh[0] = fsq;
    fsqSetup = MeshSetup::FullScreenQuad(true);
    this->dispDrawState.Mesh[0] = Gfx::CreateResource(fsqSetup);

    // draw state for rendering the final result to screen
    Id shd = Gfx::CreateResource(DisplayShader::Setup());
    auto ps = PipelineSetup::FromLayoutAndShader(fsqSetup.Layout, shd);
    ps.RasterizerState.CullFaceEnabled = false;
    ps.DepthStencilState.DepthCmpFunc = CompareFunc::Always;
    this->dispDrawState.Pipeline = Gfx::CreateResource(ps);

    // setup 2 ping-poing fp32 render targets which hold the current fractal state,
    // and the texture blocks which use reference these render targets
    this->recreateRenderTargets(Gfx::DisplayAttrs());

    // setup mandelbrot state
    shd = Gfx::CreateResource(Mandelbrot::Setup());
    ps = PipelineSetup::FromLayoutAndShader(fsqSetup.Layout, shd);
    ps.RasterizerState.CullFaceEnabled = false;
    ps.DepthStencilState.DepthCmpFunc = CompareFunc::Always;
    ps.BlendState.ColorFormat = PixelFormat::RGBA32F;
    ps.BlendState.DepthFormat = PixelFormat::None;
    this->mandelbrot.drawState.Pipeline = Gfx::CreateResource(ps);

    // setup julia state
    shd = Gfx::CreateResource(Julia::Setup());
    ps = PipelineSetup::FromLayoutAndShader(fsqSetup.Layout, shd);
    ps.RasterizerState.CullFaceEnabled = false;
    ps.DepthStencilState.DepthCmpFunc = CompareFunc::Always;
    ps.BlendState.ColorFormat = PixelFormat::RGBA32F;
    ps.BlendState.DepthFormat = PixelFormat::None;
    this->julia.drawState.Pipeline = Gfx::CreateResource(ps);

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
    this->clearFlag = false;
    ImVec2 mousePos = ImGui::GetMousePos();

    IMUI::NewFrame();

    // draw the controls window
    ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiSetCond_Once);
    ImGui::Begin("Controls", nullptr, ImVec2(300, 230));
    ImGui::BulletText("mouse-drag a rectangle to zoom in");
    ImGui::BulletText("click into Mandelbrot to render\nJulia set at that point");
    ImGui::Separator();
    if (Julia == this->fractalType) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.6f));
        if (ImGui::Button("<< Back")) {
            this->clearFlag = true;
            this->fractalType = Mandelbrot;
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();
    }
    this->clearFlag |= ImGui::Button("Redraw");
    if (ImGui::SameLine(), ImGui::Button("Zoomout")) {
        this->clearFlag |= true;
        this->zoomOut();
    }
    if (ImGui::SameLine(), ImGui::Button("Reset")) {
        this->clearFlag |= true;
        this->reset();
    }
    ImGui::SliderFloat("Colors", &this->dispFSParams.NumColors, 2.0f, 256.0f);
    ImGui::Separator();
    glm::vec2 curPos = this->convertPos(mousePos.x, mousePos.y, this->getBounds(this->fractalType));
    ImGui::Text("X: %f, Y: %f\n", curPos.x, curPos.y);
    ImGui::Separator();
    if (Mandelbrot == this->fractalType) {
        ImGui::Text("Type: Mandelbrot\n"
                    "x0: %f\ny0: %f\nx0: %f\ny1: %f",
                    this->mandelbrot.vsParams.Rect.x,
                    this->mandelbrot.vsParams.Rect.y,
                    this->mandelbrot.vsParams.Rect.w,
                    this->mandelbrot.vsParams.Rect.z);
    }
    else {
        ImGui::Text("Type: Julia\n"
                    "Pos: %f, %f\n"
                    "x0: %f\ny0: %f\nx0: %f\ny1: %f",
                    this->julia.fsParams.JuliaPos.x,
                    this->julia.fsParams.JuliaPos.y,
                    this->julia.vsParams.Rect.x,
                    this->julia.vsParams.Rect.y,
                    this->julia.vsParams.Rect.w,
                    this->julia.vsParams.Rect.z);
    }

    // handle dragging
    if (!ImGui::IsMouseHoveringAnyWindow() && ImGui::IsMouseClicked(0)) {
        this->dragStarted = true;
        this->dragStartPos = ImGui::GetMousePos();
    }
    if (this->dragStarted) {
        const ImVec2& mousePos = ImGui::GetMousePos();
        if (ImGui::IsMouseReleased(0)) {
            this->dragStarted = false;
            if ((this->dragStartPos.x != mousePos.x) && (this->dragStartPos.y != mousePos.y)) {
                this->updateFractalRect(this->dragStartPos.x, this->dragStartPos.y, mousePos.x, mousePos.y);
            }
            else {
                if (Mandelbrot == this->fractalType) {
                    // single click in Mandelbrot: render a Julia set from that point
                    this->updateJuliaPos(mousePos.x, mousePos.y);
                    this->fractalType = Julia;
                    this->zoomOut();
                }
            }
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
FractalApp::zoomOut() {
    this->setBounds(this->fractalType, glm::vec4(-2.0, -2.0, 2.0, 2.0));
}

//------------------------------------------------------------------------------
void
FractalApp::reset() {
    this->fractalType = Mandelbrot;
    this->dispFSParams.NumColors = 8.0f;
    this->zoomOut();
}

//------------------------------------------------------------------------------
void
FractalApp::setBounds(Type type, const glm::vec4& bounds) {
    if (Mandelbrot == type) {
        this->mandelbrot.vsParams.Rect = bounds;
    }
    else {
        this->julia.vsParams.Rect = bounds;
    }
}

//------------------------------------------------------------------------------
glm::vec4
FractalApp::getBounds(Type type) const {
    if (Mandelbrot == type) {
        return this->mandelbrot.vsParams.Rect;
    }
    else {
        return this->julia.vsParams.Rect;
    }
}

//------------------------------------------------------------------------------
glm::vec2
FractalApp::convertPos(float x, float y, const glm::vec4& bounds) const {
    // convert mouse pos to fractal real/imaginary pos
    const DisplayAttrs& attrs = Gfx::DisplayAttrs();
    const float fbWidth = (float) attrs.FramebufferWidth;
    const float fbHeight = (float) attrs.FramebufferHeight;
    glm::vec2 rel = glm::vec2(x, y) / glm::vec2(fbWidth, fbHeight);
    return glm::vec2(bounds.x + ((bounds.z - bounds.x) * rel.x),
                     bounds.y + ((bounds.w - bounds.y) * rel.y));
}

//------------------------------------------------------------------------------
void
FractalApp::updateFractalRect(float x0, float y0, float x1, float y1) {

    if ((x0 == x1) || (y0 == y1)) return;
    if (x0 > x1) std::swap(x0, x1);
    if (y0 > y1) std::swap(y0, y1);
    glm::vec4 bounds = this->getBounds(this->fractalType);
    glm::vec2 topLeft = this->convertPos(x0, y0, bounds);
    glm::vec2 botRight = this->convertPos(x1, y1, bounds);
    this->setBounds(this->fractalType, glm::vec4(topLeft, botRight));
}

//------------------------------------------------------------------------------
void
FractalApp::updateJuliaPos(float x, float y) {
    this->julia.fsParams.JuliaPos = this->convertPos(x, y, this->getBounds(this->fractalType));
}

//------------------------------------------------------------------------------
void
FractalApp::recreateRenderTargets(const DisplayAttrs& attrs) {
    // window size has changed, re-create render targets
    Log::Info("(re-)create render targets\n");

    // first destroy previous render targets
    if (ResourceLabel::Invalid != this->offscreenRenderTargetLabel) {
        Gfx::DestroyResources(this->offscreenRenderTargetLabel);
    }
    this->offscreenRenderTargetLabel = Gfx::PushResourceLabel();
    auto offscreenRTSetup = TextureSetup::RenderTarget(attrs.FramebufferWidth, attrs.FramebufferHeight);
    offscreenRTSetup.ColorFormat = PixelFormat::RGBA32F;
    offscreenRTSetup.DepthFormat = PixelFormat::None;
    offscreenRTSetup.Sampler.MinFilter = TextureFilterMode::Nearest;
    offscreenRTSetup.Sampler.MagFilter = TextureFilterMode::Nearest;
    offscreenRTSetup.ClearHint = this->fractalClearState;
    this->offscreenRenderTarget[0] = Gfx::CreateResource(offscreenRTSetup);
    this->offscreenRenderTarget[1] = Gfx::CreateResource(offscreenRTSetup);
    this->clearFlag = true;

    Gfx::PopResourceLabel();
}
