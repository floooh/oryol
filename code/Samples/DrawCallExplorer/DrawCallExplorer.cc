//------------------------------------------------------------------------------
//  DrawCallExplorer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Core/Time/Clock.h"
#include "Gfx/Gfx.h"
#include "Assets/Gfx/ShapeBuilder.h"
#include "Input/Input.h"
#include "IMUI/IMUI.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/random.hpp"
#include "shaders.h"

using namespace Oryol;

class DrawCallExplorerApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
    
private:
    void updateCamera();
    void emitParticles();
    void updateParticles();
    void drawUI();
    void reset();

    int maxNumParticles = 10000;
    int numEmitParticles = 100;
    int curNumParticles = 0;
    int numParticlesPerBatch = 1000;

    Id mesh;
    StaticArray<Id,3> pipelines;

    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 model;

    // FIXME: hmm these param blocks are actually compatibel across shaders
    RedShader::PerFrameParams perFrameParams;
    RedShader::PerParticleParams perParticleParams;

    static const int ParticleBufferSize = 1000000;
    bool updateEnabled = true;
    int frameCount = 0;
    TimePoint lastFrameTimePoint;
    struct {
        glm::vec4 pos;
        glm::vec4 vec;
    } particles[ParticleBufferSize];

    // frameInfo before rendering UI and before Commit
    GfxFrameInfo frameInfoBeforeUI;
    GfxFrameInfo frameInfoBeforeCommit;
    GfxFrameInfo lastFrameInfoBeforeUI;
    GfxFrameInfo lastFrameInfoBeforeCommit;

    // NumSamples must be 2^N, all samples are 0.0 to 1.0 (0..32ms)
    static const int NumSamples = 1024;
    uint32_t curSample = 0;
    struct sample {
        float updTime = 0.0f;
        float applyRtTime = 0.0f;
        float drawTime = 0.0f;
        float uiTime = 0.0f;
        float commitTime = 0.0f;
        float frameTime = 0.0f;
    };
    sample samples[NumSamples];
};
OryolMain(DrawCallExplorerApp);

//------------------------------------------------------------------------------
AppState::Code
DrawCallExplorerApp::OnRunning() {

    TimePoint start, afterUpdate, afterApplyRt, afterDraw, afterUi, afterCommit;
    this->frameCount++;

    // update block
    start = Clock::Now();
    this->updateCamera();
    if (this->updateEnabled) {
        this->emitParticles();
        this->updateParticles();
    }
    afterUpdate = Clock::Now();

    // render block
    Gfx::ApplyDefaultRenderTarget(ClearState::ClearAll(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), 1.0f, 0));
    afterApplyRt = Clock::Now();
    int batchCount = this->numParticlesPerBatch;
    int curBatch = 0;
    DrawState drawState;
    drawState.Mesh[0] = this->mesh;
    for (int i = 0; i < this->curNumParticles; i++) {
        if (++batchCount >= this->numParticlesPerBatch) {
            batchCount = 0;
            drawState.Pipeline = this->pipelines[curBatch++];
            Gfx::ApplyDrawState(drawState);
            Gfx::ApplyUniformBlock(this->perFrameParams);
            if (curBatch >= 3) {
                curBatch = 0;
            }
        }
        this->perParticleParams.Translate = this->particles[i].pos;
        Gfx::ApplyUniformBlock(this->perParticleParams);
        Gfx::Draw(0);
    }
    afterDraw = Clock::Now();
    this->frameInfoBeforeUI = Gfx::FrameInfo();

    this->drawUI();
    afterUi = Clock::Now();
    this->frameInfoBeforeCommit = Gfx::FrameInfo();
    Gfx::CommitFrame();
    afterCommit = Clock::Now();

    // record time samples
    auto& s = this->samples[this->curSample++ & (NumSamples-1)];
    s.updTime = (float) (afterUpdate - start).AsMilliSeconds();
    s.applyRtTime = (float) (afterApplyRt - afterUpdate).AsMilliSeconds();
    s.drawTime = (float) (afterDraw - afterApplyRt).AsMilliSeconds();
    s.uiTime = (float) (afterUi - afterDraw).AsMilliSeconds();
    s.commitTime = (float) (afterCommit - afterUi).AsMilliSeconds();
    s.frameTime = (float) Clock::LapTime(this->lastFrameTimePoint).AsMilliSeconds();

    this->lastFrameInfoBeforeCommit = this->frameInfoBeforeCommit;
    this->lastFrameInfoBeforeUI = this->frameInfoBeforeUI;

    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
void
DrawCallExplorerApp::updateCamera() {
    float angle = this->frameCount * 0.01f;
    glm::vec3 pos(glm::sin(angle) * 10.0f, 2.5f, glm::cos(angle) * 10.0f);
    this->view = glm::lookAt(pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    this->perFrameParams.ModelViewProjection = this->proj * this->view * this->model;
}

//------------------------------------------------------------------------------
void
DrawCallExplorerApp::emitParticles() {
    for (int i = 0;
        (this->curNumParticles < this->maxNumParticles) &&
        (i < this->numEmitParticles);
        i++) {
        
        this->particles[this->curNumParticles].pos = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 rnd = glm::ballRand(0.5f);
        rnd.y += 2.0f;
        this->particles[this->curNumParticles].vec = glm::vec4(rnd, 0.0f);
        this->curNumParticles++;
    }
}

//------------------------------------------------------------------------------
void
DrawCallExplorerApp::updateParticles() {
    const float frameTime = 1.0f / 60.0f;
    for (int i = 0; i < this->curNumParticles; i++) {
        auto& curParticle = this->particles[i];
        curParticle.vec.y -= 1.0f * frameTime;
        curParticle.pos += curParticle.vec * frameTime;
        if (curParticle.pos.y < -2.0f) {
            curParticle.pos.y = -1.8f;
            curParticle.vec.y = -curParticle.vec.y;
            curParticle.vec *= 0.8f;
        }
    }
}

//------------------------------------------------------------------------------
void
DrawCallExplorerApp::reset() {
    this->curNumParticles = 0;
    this->curSample = 0;
    for (auto& s : this->samples) {
        s = sample();
    }
}

//------------------------------------------------------------------------------
AppState::Code
DrawCallExplorerApp::OnInit() {
    // setup rendering system
    GfxSetup gfxSetup = GfxSetup::Window(620, 500, "Oryol DrawCallExplorer");
    gfxSetup.GlobalUniformBufferSize = 1024 * 1024 * 32;
    Gfx::Setup(gfxSetup);
    Input::Setup();
    Input::BeginCaptureText();
    IMUI::Setup();

    // create resources
    const glm::mat4 rot90 = glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ShapeBuilder shapeBuilder;
    shapeBuilder.RandomColors = true;
    shapeBuilder.Layout
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::Color0, VertexFormat::Float4);
    shapeBuilder.Transform(rot90).Sphere(0.05f, 3, 2);
    this->mesh = Gfx::CreateResource(shapeBuilder.Build());

    Id redShd   = Gfx::CreateResource(RedShader::Setup());
    Id greenShd = Gfx::CreateResource(GreenShader::Setup());
    Id blueShd  = Gfx::CreateResource(BlueShader::Setup());

    auto ps = PipelineSetup::FromLayoutAndShader(shapeBuilder.Layout, redShd);
    ps.RasterizerState.CullFaceEnabled = true;
    ps.DepthStencilState.DepthWriteEnabled = true;
    ps.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    this->pipelines[0] = Gfx::CreateResource(ps);
    ps.Shader = greenShd;
    this->pipelines[1] = Gfx::CreateResource(ps);
    ps.Shader = blueShd;
    this->pipelines[2] = Gfx::CreateResource(ps);
    
    // setup projection and view matrices
    const float fbWidth = (const float) Gfx::DisplayAttrs().FramebufferWidth;
    const float fbHeight = (const float) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->view = glm::lookAt(glm::vec3(0.0f, 2.5f, 0.0f), glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    this->model = glm::mat4();

    this->lastFrameTimePoint = Clock::Now();
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
DrawCallExplorerApp::OnCleanup() {
    IMUI::Discard();
    Input::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
void
DrawCallExplorerApp::drawUI() {
    IMUI::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiSetCond_Once);
    ImGui::SetNextWindowSize(ImVec2(250, 160), ImGuiSetCond_Once);
    if (ImGui::Begin("Controls")) {
        if (ImGui::Button("Reset")) {
            this->reset();
        }
        ImGui::PushItemWidth(100.0f);
        if (ImGui::InputInt("Max Particles", &this->maxNumParticles, 1000, 10000, ImGuiInputTextFlags_EnterReturnsTrue)) {
            if (this->maxNumParticles > ParticleBufferSize) {
                this->maxNumParticles = ParticleBufferSize;
            }
            this->reset();
        }
        if (ImGui::InputInt("Batch Size", &this->numParticlesPerBatch, 100, 1000, ImGuiInputTextFlags_EnterReturnsTrue)) {
            this->reset();
        }
        if (ImGui::InputInt("Emit Per Frame", &this->numEmitParticles, 10, 100, ImGuiInputTextFlags_EnterReturnsTrue)) {
            this->reset();
        }
        ImGui::Text("Num ApplyDrawState: %d (ui: %d)",
            this->lastFrameInfoBeforeCommit.NumApplyDrawState,
            this->lastFrameInfoBeforeCommit.NumApplyDrawState - this->lastFrameInfoBeforeUI.NumApplyDrawState);
        ImGui::Text("Num DrawCalls:      %d (ui: %d)",
            this->lastFrameInfoBeforeCommit.NumDraw,
            this->lastFrameInfoBeforeCommit.NumDraw - this->lastFrameInfoBeforeUI.NumDraw);
        ImGui::PopItemWidth();
    }
    ImGui::End();
    ImGui::SetNextWindowPos(ImVec2(10, 290), ImGuiSetCond_Once);
    ImGui::SetNextWindowSize(ImVec2(600, 200), ImGuiSetCond_Once);
    if (ImGui::Begin("Timing")) {
        const auto& s0 = this->samples[0];
        const auto& s1 = this->samples[(this->curSample-1) & (NumSamples-1)];
        ImGui::PlotLines("##upd", &s0.updTime, NumSamples, 0, nullptr, 0.0f, 32.0f, ImVec2(450, 24), sizeof(sample));
        ImGui::SameLine(); ImGui::Text("update:  %.3fms", s1.updTime);
        ImGui::PlotLines("##applyRt", &s0.applyRtTime, NumSamples, 0, nullptr, 0.0f, 32.0f, ImVec2(450, 24), sizeof(sample));
        ImGui::SameLine(); ImGui::Text("applyRt: %.3fms", s1.applyRtTime);
        ImGui::PlotLines("##draw", &s0.drawTime, NumSamples, 0, nullptr, 0.0f, 32.0f, ImVec2(450, 24), sizeof(sample));
        ImGui::SameLine(); ImGui::Text("draw:    %.3fms", s1.drawTime);
        ImGui::PlotLines("##ui", &s0.uiTime, NumSamples, 0, nullptr, 0.0f, 32.0f, ImVec2(450, 24), sizeof(sample));
        ImGui::SameLine(); ImGui::Text("ui:      %.3fms", s1.uiTime);
        ImGui::PlotLines("##commit", &s0.commitTime, NumSamples, 0, nullptr, 0.0f, 32.0f, ImVec2(450, 24), sizeof(sample));
        ImGui::SameLine(); ImGui::Text("commit:  %.3fms", s1.commitTime);
        ImGui::PlotLines("##frame", &s0.frameTime, NumSamples, 0, nullptr, 0.0f, 32.0f, ImVec2(450, 24), sizeof(sample));
        ImGui::SameLine(); ImGui::Text("frame:   %.3fms", s1.frameTime);
    }
    ImGui::End();
    ImGui::Render();
}
