//------------------------------------------------------------------------------
//  GamepadExplorer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "Dbg/Dbg.h"
#include "Input/Input.h"

using namespace Oryol;

class GamepadExplorerApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();
};
OryolMain(GamepadExplorerApp);

//------------------------------------------------------------------------------
AppState::Code
GamepadExplorerApp::OnInit() {
    Gfx::Setup(GfxSetup::Window(800, 600, "Test Gamepads"));
    Dbg::Setup();
    Input::Setup();
    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
GamepadExplorerApp::OnRunning() {
    Gfx::ApplyDefaultRenderTarget();
    
    // Gamepad0 Gamepad1 Gamepad2 Gamepad3
    Dbg::Print("\n\t\t");
    for (int i = 0; i < Input::MaxNumGamepads; i++) {
        if (Input::GamepadAttached(i)) {
            Dbg::TextColor(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        }
        else {
            Dbg::TextColor(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
        }
        Dbg::PrintF("Gamepad%d\t", i);
    }
    Dbg::Print("\n\n\r");

    // button status
    Dbg::TextColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
    Dbg::Print("Buttons:\t");
    for (int padIndex = 0; padIndex < Input::MaxNumGamepads; padIndex++) {
        for (int btnIndex = 0; btnIndex < Input::MaxNumRawButtons; btnIndex++) {
            if (Input::GamepadRawButtonPressed(padIndex, btnIndex)) {
                Dbg::PrintF("%d ", btnIndex);
            }
        }
        Dbg::Print("\t\t");
    }
    Dbg::Print("\n\n\r");

    // axis status
    for (int axisIndex = 0; axisIndex < Input::MaxNumRawAxes; axisIndex++) {
        Dbg::PrintF("Axis %2d:\t", axisIndex);
        for (int padIndex = 0; padIndex < Input::MaxNumGamepads; padIndex++) {
            Dbg::PrintF("%.3f\t\t", Input::GamepadRawAxisValue(padIndex, axisIndex));
        }
        Dbg::Print("\n\r");
    }
    Dbg::Print("\n\n\r");

    // gamepad id's
    for (int padIndex = 0; padIndex < Input::MaxNumGamepads; padIndex++) {
        Dbg::PrintF("Gamepad%d Id:\t%s\n\r", padIndex, Input::GamepadTypeId(padIndex).AsCStr());
    }

    Dbg::DrawTextBuffer();
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
GamepadExplorerApp::OnCleanup() {
    Input::Discard();
    Dbg::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}