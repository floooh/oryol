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

const char* btnNames[GamepadButton::NumButtons] = {
    "A              ",
    "B              ",
    "X              ",
    "Y              ",
    "LeftBumper     ",
    "RightBumper    ",
    "LeftTrigger    ",
    "RightTrigger   ",
    "Back           ",
    "Start          ",
    "LeftStick      ",
    "RightStick     ",
    "DPadUp         ",
    "DPadDown       ",
    "DPadLeft       ",
    "DPadRight      ",
    "Center         "
};

const char* axisNames[GamepadAxis::NumAxes] = {
    "LeftStickHori  ",
    "LeftStickVert  ",
    "RightStickHori ",
    "RightStickVert ",
    "LeftTrigger    ",
    "RightTrigger   "
};

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
    Gfx::BeginPass();
    
    // Gamepad0 Gamepad1 Gamepad2 Gamepad3
    Dbg::Print("\n\t\t");
    for (int i = 0; i < Input::MaxNumGamepads; i++) {
        if (Input::GamepadAttached(i)) {
            Dbg::TextColor(0.0f, 1.0f, 0.0f, 1.0f);
        }
        else {
            Dbg::TextColor(0.5f, 0.5f, 0.5f, 1.0f);
        }
        Dbg::PrintF("Gamepad%d\t", i);
    }
    Dbg::Print("\n\n\r");

    // button status
    Dbg::TextColor(1.0f, 1.0f, 0.0f, 1.0f);
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

    // mapped buttons
    Dbg::Print("\n\n");
    for (int btnIndex = 0; btnIndex < GamepadButton::NumButtons; btnIndex++) {
        Dbg::PrintF("%s\t", btnNames[btnIndex]);
        for (int padIndex = 0; padIndex < Input::MaxNumGamepads; padIndex++) {
            Dbg::PrintF("%s\t\t", Input::GamepadButtonPressed(padIndex, (GamepadButton::Code)btnIndex) ? "X":"-");
        }
        Dbg::Print("\n\r");
    }

    // mapped axis values
    Dbg::Print("\n");
    for (int axisIndex = 0; axisIndex < GamepadAxis::NumAxes; axisIndex++) {
        Dbg::PrintF("%s\t", axisNames[axisIndex]);
        for (int padIndex = 0; padIndex < Input::MaxNumGamepads; padIndex++) {
            Dbg::PrintF("%.3f\t\t", Input::GamepadAxisValue(padIndex, (GamepadAxis::Code)axisIndex));
        }
        Dbg::Print("\n\r");
    }

    Dbg::DrawTextBuffer();
    Gfx::EndPass();
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