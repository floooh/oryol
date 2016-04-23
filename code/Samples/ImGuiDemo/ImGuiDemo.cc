//------------------------------------------------------------------------------
//  ImGuiDemo.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Core/Time/Clock.h"
#include "Gfx/Gfx.h"
#include "Input/Input.h"
#include "IMUI/IMUI.h"

using namespace Oryol;

class ImGuiDemoApp : public App {
public:
    /// on init frame method
    virtual AppState::Code OnInit();
    /// on running frame method
    virtual AppState::Code OnRunning();
    /// on cleanup frame method
    virtual AppState::Code OnCleanup();

private:
    bool showTestWindow = true;
    bool showAnotherWindow = false;
    ImVec4 clearColor = ImColor(114, 144, 154);
    TimePoint lastTimePoint;
    ClearState clearState;
};
OryolMain(ImGuiDemoApp);

//------------------------------------------------------------------------------
AppState::Code
ImGuiDemoApp::OnInit() {
    Gfx::Setup(GfxSetup::Window(1024, 700, "Oryol ImGui Demo"));
    Input::Setup();
    Input::BeginCaptureText();
    IMUI::Setup();
    this->lastTimePoint = Clock::Now();

    return AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
ImGuiDemoApp::OnRunning() {

    this->clearState.Color = glm::vec4(this->clearColor.x, this->clearColor.y, this->clearColor.z, 1.0f);
    Gfx::ApplyDefaultRenderTarget(this->clearState);
    IMUI::NewFrame(Clock::LapTime(this->lastTimePoint));

    // 1. Show a simple window
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
    static float f = 0.0f;
    ImGui::Text("Hello, world!");
    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
    ImGui::ColorEdit3("clear color", (float*)&this->clearColor);
    if (ImGui::Button("Test Window")) this->showTestWindow ^= 1;
    if (ImGui::Button("Another Window")) this->showAnotherWindow ^= 1;
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    // 2. Show another simple window, this time using an explicit Begin/End pair
    if (this->showAnotherWindow) {
        ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
        ImGui::Begin("Another Window", &this->showAnotherWindow);
        ImGui::Text("Hello");
        ImGui::End();
    }

    // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
    if (this->showTestWindow) {
        ImGui::SetNextWindowPos(ImVec2(460, 20), ImGuiSetCond_FirstUseEver);
        ImGui::ShowTestWindow(&showTestWindow);
    }

    ImGui::Render();
    Gfx::CommitFrame();

    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
ImGuiDemoApp::OnCleanup() {
    IMUI::Discard();
    Input::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}