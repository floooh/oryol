//------------------------------------------------------------------------------
//  TestInput.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "Assets/Gfx/ShapeBuilder.h"
#include "Dbg/Dbg.h"
#include "Input/Input.h"
#include "Core/String/StringConverter.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/polar_coordinates.hpp"
#include "shaders.h"
#include <float.h>

using namespace Oryol;

class TestInputApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();

    void testMouseButton(MouseButton::Code btn, const char* name);
    void testKey(Key::Code key, const char* name);
    void printMouseState();
    void printKeyboardState();
    void printTouchpadState();
    void printSensorState();
    void printGamepadState(int gamepadIndex);
    glm::vec4 getClearColor();
    void updateView();
    void reset();
    void drawCube();
    void handleKeyboardInput();
    void handleMouseInput();
    void handleTouchInput();
    void handleGamepadInput(int gamepadIndex);
    
    const float downColor[4]{1.0f, 0.0f, 0.0f, 1.0f};
    const float upColor[4]{0.0f, 0.0f, 1.0f, 1.0f};
    const float pressedColor[4]{0.0f, 1.0f, 0.0f, 1.0f};
    const float defaultColor[4]{1.0f, 1.0f, 1.0f, 0.5f};
    float minLatitude;
    float maxLatitude;
    float minDist;
    float maxDist;

    DrawState drawState;
    glm::vec2 startPolar;
    glm::vec2 polar;
    float distance = 6.0f;
    float startDistance = 6.0f;
    glm::vec2 startMousePos;
    glm::vec3 pointOfInterest;
    glm::mat4 proj;
    glm::mat4 view;
    glm::mat4 invView;
    bool pointerLock = false;
    String lastCaptured;
    int selectedGamepadIndex = 0;
};
OryolMain(TestInputApp);

//------------------------------------------------------------------------------
AppState::Code
TestInputApp::OnInit() {
    auto gfxSetup = GfxSetup::Window(800, 400, "Oryol Input Test Sample");
    gfxSetup.HighDPI = true;
    Gfx::Setup(gfxSetup);
    Dbg::Setup();
    if (Gfx::DisplayAttrs().WindowWidth > 800) {
        Dbg::TextScale(2.0f, 2.0f);
    }

    Input::Setup();
    Input::SetPointerLockHandler([this](const InputEvent& event) -> PointerLockMode::Code {
        if (event.Button == MouseButton::Left) {
            if (event.Type == InputEvent::MouseButtonDown) {
                this->pointerLock = true;
                return PointerLockMode::Enable;
            }
            else if (event.Type == InputEvent::MouseButtonUp) {
                this->pointerLock = false;
                return PointerLockMode::Disable;
            }
        }
        return PointerLockMode::DontCare;
    });
    
    // create a 3D cube
    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout = {
        { VertexAttr::Position, VertexFormat::Float3 },
        { VertexAttr::Normal, VertexFormat::Byte4N }
    };
    shapeBuilder.Box(1.0f, 1.0f, 1.0f, 1);
    this->drawState.Mesh[0] = Gfx::CreateResource(shapeBuilder.Build());
    Id shd = Gfx::CreateResource(Shader::Setup());
    auto ps = PipelineSetup::FromLayoutAndShader(shapeBuilder.Layout, shd);
    ps.DepthStencilState.DepthWriteEnabled = true;
    ps.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    ps.RasterizerState.CullFaceEnabled = true;
    this->drawState.Pipeline = Gfx::CreateResource(ps);

    const float fbWidth = (const float) Gfx::DisplayAttrs().FramebufferWidth;
    const float fbHeight = (const float) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);
    this->polar = glm::vec2(glm::radians(45.0f), glm::radians(45.0f));
    this->distance = 6.0f;
    this->minLatitude = glm::radians(-85.0f);
    this->maxLatitude = glm::radians(+85.0f);
    this->minDist = 1.5f;
    this->maxDist = 20.0f;
    
    return App::OnInit();
}

//------------------------------------------------------------------------------
void
TestInputApp::updateView() {
    // first rotate around global Y axis, then rotate around global X axis (transformed to local space)
    glm::vec3 eucl = glm::euclidean(this->polar) * distance;
    this->view = glm::lookAt(eucl + this->pointOfInterest, this->pointOfInterest, glm::vec3(0.0f, 1.0f, 0.0f));
    this->invView = glm::inverse(this->view);
}

//------------------------------------------------------------------------------
void
TestInputApp::testMouseButton(MouseButton::Code btn, const char* name) {
    glm::vec4 color;
    if (Input::MouseButtonDown(btn)) Dbg::TextColor(this->downColor);
    else if (Input::MouseButtonUp(btn)) Dbg::TextColor(this->upColor);
    else if (Input::MouseButtonPressed(btn)) Dbg::TextColor(this->pressedColor);
    else Dbg::TextColor(this->defaultColor);
    Dbg::PrintF(" %s", name);
}

//------------------------------------------------------------------------------
void
TestInputApp::testKey(Key::Code key, const char* name) {
    glm::vec4 color;
    if (Input::KeyDown(key)) {
        Dbg::TextColor(this->downColor);
        Dbg::PrintF(" %s", Key::ToString(key));
    }
    else if (Input::KeyUp(key)) {
        Dbg::TextColor(this->upColor);
        Dbg::PrintF(" %s", Key::ToString(key));
    }
    else if (Input::KeyPressed(key)) {
        Dbg::TextColor(this->pressedColor);
        Dbg::PrintF(" %s", Key::ToString(key));
    }
}

//------------------------------------------------------------------------------
void
TestInputApp::printMouseState() {
    if (Input::MouseAttached()) {
        Dbg::TextColor(1.0f, 0.0f, 0.0f, 1.0f);
        Dbg::Print("\n MOUSE STATUS (LMB for pointerlock):\n\n\r");
        
        this->testMouseButton(MouseButton::Left, "LMB");
        this->testMouseButton(MouseButton::Middle, "MMB");
        this->testMouseButton(MouseButton::Right, "RMB");
        if (this->pointerLock) {
            Dbg::TextColor(this->pressedColor);
        }
        else {
            Dbg::TextColor(this->defaultColor);
        }
        Dbg::PrintF(" POINTERLOCK");

        Dbg::TextColor(1.0f, 1.0f, 1.0f, 1.0f);
        Dbg::PrintF("\n\r pos: %.3f %.3f\n\r mov: %.3f %.3f\n\r scroll: %.3f %.3f",
                    Input::MousePosition().x, Input::MousePosition().y,
                    Input::MouseMovement().x, Input::MouseMovement().y,
                    Input::MouseScroll().x, Input::MouseScroll().y);
    }
    else {
        Dbg::TextColor(1.0f, 0.0f, 0.0f, 1.0f);
        Dbg::Print("\n MOUSE NOT ATTACHED");
    }
}

//------------------------------------------------------------------------------
void
TestInputApp::printKeyboardState() {
    if (Input::KeyboardAttached()) {
        Dbg::TextColor(1.0f, 1.0f, 0.0f, 1.0f);
        Dbg::Print("\n\n\r KEYBOARD STATUS:\n\n\r");
        Dbg::Print(" keys: ");
        for (int key = 0; key < Key::NumKeys; key++) {
            this->testKey((Key::Code)key, Key::ToString((Key::Code)key));
        }
        if (Input::Text()[0]) {
            this->lastCaptured = StringConverter::WideToUTF8(Input::Text());
        }
        Dbg::PrintF("\n\n\r last char: %s\n\r", this->lastCaptured.AsCStr());
    }
    else {
        Dbg::TextColor(1.0f, 0.0f, 0.0f, 1.0f);
        Dbg::Print("\n\n\r KEYBOARD NOT ATTACHED");
    };
}

//------------------------------------------------------------------------------
void
TestInputApp::printTouchpadState() {
    if (Input::TouchpadAttached()) {
        Dbg::TextColor(1.0f, 0.0f, 0.0f, 1.0f);
        Dbg::Print("\n\n\r TOUCHPAD STATUS:\n\n\r");
        if (Input::TouchTapped()) {
            Dbg::TextColor(this->pressedColor);
        }
        else {
            Dbg::TextColor(this->defaultColor);
        }
        Dbg::Print(" TAPPED ");
        if (Input::TouchDoubleTapped()) {
            Dbg::TextColor(this->pressedColor);
        }
        else {
            Dbg::TextColor(this->defaultColor);
        }
        Dbg::Print("DOUBLETAPPED ");
        if (Input::TouchPanning()) {
            Dbg::TextColor(this->pressedColor);
        }
        else {
            Dbg::TextColor(this->defaultColor);
        }
        Dbg::Print("PANNING ");
        if (Input::TouchPinching()) {
            Dbg::TextColor(this->pressedColor);
        }
        else {
            Dbg::TextColor(this->defaultColor);
        }
        Dbg::Print("PINCHING");
        Dbg::Print("\n\n\r");
        Dbg::TextColor(1.0f, 1.0f, 1.0f, 1.0f);
        Dbg::PrintF(" touch position0: %.3f %.3f\n\r"
                    " touch movement0: %.3f %.3f\n\r"
                    " touch startPos0: %.3f %.3f\n\r"
                    " touch position1: %.3f %.3f\n\r"
                    " touch movement1: %.3f %.3f\n\r"
                    " touch startPos1: %.3f %.3f\n\r",
                    Input::TouchPosition(0).x, Input::TouchPosition(0).y,
                    Input::TouchMovement(0).x, Input::TouchMovement(0).y,
                    Input::TouchStartPosition(0).x, Input::TouchStartPosition(0).y,
                    Input::TouchPosition(1).x, Input::TouchPosition(1).y,
                    Input::TouchMovement(1).x, Input::TouchMovement(1).y,
                    Input::TouchStartPosition(1).x, Input::TouchStartPosition(1).y);
    }
    else {
        Dbg::TextColor(1.0f, 0.0f, 0.0f, 1.0f);
        Dbg::Print("\n\n\r TOUCHPAD NOT ATTACHED");
    }
}

//------------------------------------------------------------------------------
void
TestInputApp::printSensorState() {
    if (Input::SensorsAttached()) {
        Dbg::TextColor(1.0f, 0.0f, 0.0f, 1.0f);
        Dbg::Print("\n\n\r SENSOR STATUS:\n\n\r");
        Dbg::TextColor(1.0f, 1.0f, 1.0f, 1.0f);
        const glm::vec3& acc = Input::SensorAcceleration();
        const glm::vec3& ypr = Input::SensorYawPitchRoll();
        Dbg::PrintF(" acceleration: %.3f %.3f %.3f\n\r", acc.x, acc.y, acc.z);
        Dbg::PrintF(" yaw: %.3f, pitch: %.3f, roll: %.3f\n\r",
                    glm::degrees(ypr.x), glm::degrees(ypr.y), glm::degrees(ypr.z));
    }
    else {
        Dbg::TextColor(1.0f, 0.0f, 0.0f, 1.0f);
        Dbg::Print("\n\n\r SENSORS NOT ATTACHED");
    }
}

//------------------------------------------------------------------------------
void
TestInputApp::printGamepadState(int gamepadIndex) {
    // TODO: range check incoming gamepadIndex against maximum num game pads and zero
    //assert(gamepadIndex >= 0 && gamepadIndex < GamePad::NUM)

    if (Input::GamepadAttached(gamepadIndex)) {
        Dbg::TextColor(1.0f, 0.0f, 0.0f, 1.0f);
        Dbg::PrintF("\n\n\r GAMEPAD %d STATUS:\n\n\r", gamepadIndex);
        Dbg::TextColor(1.0f, 1.0f, 1.0f, 1.0f);
        Dbg::PrintF(" Id: %s\n\r", Input::GamepadTypeId(gamepadIndex).AsCStr());
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadButton::A) ? " A" : "  ");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadButton::B) ? " B" : "  ");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadButton::X) ? " X" : "  ");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadButton::Y) ? " Y\n\r" : "\n\r");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadButton::Start) ? " Start" : "      ");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadButton::Back) ? " Back\n\r" : "\n\r");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadButton::LeftBumper) ? " LeftBumper\t" : "            ");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadButton::RightBumper) ? " RightBumper\n\r" : "\n\r");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadButton::DPadLeft) ? " DPadLeft" : "         ");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadButton::DPadRight) ? " DPadRight" : "          ");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadButton::DPadUp) ? " DPadUp" : "       ");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadButton::DPadDown) ? " DPadDown\n\r" : "        \n\r");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadButton::LeftTrigger) ? " LeftTrigger" : "            ");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadButton::RightTrigger) ? " RightTrigger\n\r" : "           \n\r");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadButton::LeftStick) ? " LeftStick  " : "            ");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadButton::RightStick) ? " RightStick\n\r" : "          \n\r");
        Dbg::PrintF(" LeftStick.x %4.4f", Input::GamepadAxisValue(gamepadIndex, GamepadAxis::LeftStickHori));
        Dbg::PrintF(" LeftStick.y %4.4f\n\r", Input::GamepadAxisValue(gamepadIndex, GamepadAxis::LeftStickVert));
        Dbg::PrintF(" RightStick.x %4.4f", Input::GamepadAxisValue(gamepadIndex, GamepadAxis::RightStickHori));
        Dbg::PrintF(" RightStick.y %4.4f\n\r", Input::GamepadAxisValue(gamepadIndex, GamepadAxis::RightStickVert));
        Dbg::PrintF(" LeftTrigger %4.4f", Input::GamepadAxisValue(gamepadIndex, GamepadAxis::LeftTrigger));
        Dbg::PrintF(" RightTrigger %4.4f\n\r", Input::GamepadAxisValue(gamepadIndex, GamepadAxis::RightTrigger));
    }
    else {
        Dbg::TextColor(1.0f, 0.0f, 0.0f, 1.0f);
        Dbg::PrintF("\n\n\r GAMEPAD %d NOT ATTACHED", gamepadIndex);
    }
}

//------------------------------------------------------------------------------
glm::vec4
TestInputApp::getClearColor() {
    glm::vec4 clearColor(0.25f, 0.25f, 0.25f, 1.0f);
    if (Input::TouchTapped()) {
        return glm::vec4(this->downColor[0], this->downColor[1], this->downColor[2], this->downColor[3]);
    }
    else if (Input::TouchDoubleTapped()) {
        return glm::vec4(this->upColor[0], this->upColor[1], this->upColor[2], this->upColor[3]);
    }
    else {
        return glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
    }
}

//------------------------------------------------------------------------------
void
TestInputApp::reset() {
    this->polar = glm::vec2(glm::radians(45.0f), glm::radians(45.0f));
    this->distance = 6.0f;
}

//------------------------------------------------------------------------------
void
TestInputApp::handleKeyboardInput() {
    if (Input::KeyboardAttached()) {

        if (Input::KeyDown(Key::Space)) {
            this->reset();
        }

        static const float rotatePerFrame = 0.025f;
        static const float movePerFrame = 0.025f;
        
        if (Input::KeyPressed(Key::LeftShift)) {
            // rotate cube
            if (Input::KeyPressed(Key::Left)) {
                this->polar.y -= rotatePerFrame;
            }
            if (Input::KeyPressed(Key::Right)) {
                this->polar.y += rotatePerFrame;
            }
            if (Input::KeyPressed(Key::Up)) {
                this->polar.x = glm::clamp(this->polar.x - rotatePerFrame, this->minLatitude, this->maxLatitude);
            }
            if (Input::KeyPressed(Key::Down)) {
                this->polar.x = glm::clamp(this->polar.x + rotatePerFrame, this->minLatitude, this->maxLatitude);
            }
        }
        else {
            // move cube
            if (Input::KeyPressed(Key::Left)) {
                this->pointOfInterest += glm::vec3(this->invView[0]) * movePerFrame;
            }
            if (Input::KeyPressed(Key::Right)) {
                this->pointOfInterest -= glm::vec3(this->invView[0]) * movePerFrame;
            }
            if (Input::KeyPressed(Key::Up)) {
                this->pointOfInterest -= glm::vec3(this->invView[1]) * movePerFrame;
            }
            if (Input::KeyPressed(Key::Down)) {
                this->pointOfInterest += glm::vec3(this->invView[1]) * movePerFrame;
            }
            // select gamepad
            if (Input::KeyPressed(Key::N1)) {
            	this->selectedGamepadIndex = 0;
            }
            else if (Input::KeyPressed(Key::N2)) {
            	this->selectedGamepadIndex = 1;
            }
            else if (Input::KeyPressed(Key::N3)) {
            	this->selectedGamepadIndex = 2;
            }
            else if (Input::KeyPressed(Key::N4)) {
            	this->selectedGamepadIndex = 3;
            }
        }
    }
}

//------------------------------------------------------------------------------
void
TestInputApp::handleMouseInput() {
    if (Input::MouseAttached()) {
        if (Input::MouseButtonPressed(MouseButton::Left)) {
            this->polar.y -= Input::MouseMovement().x * 0.01f;
            this->polar.x = glm::clamp(this->polar.x + Input::MouseMovement().y * 0.01f, this->minLatitude, this->maxLatitude);
        }
        this->distance = glm::clamp(this->distance + Input::MouseScroll().y * 0.1f, this->minDist, this->maxDist);
    }
}

//------------------------------------------------------------------------------
void
TestInputApp::handleTouchInput() {
    if (Input::TouchpadAttached()) {
        if (Input::TouchDoubleTapped()) {
            this->reset();
        }
        
        if (Input::TouchPanningStarted()) {
            this->startPolar = this->polar;
        }
        if (Input::TouchPanning()) {
            glm::vec2 diff = (Input::TouchPosition(0) - Input::TouchStartPosition(0)) * 0.01f;
            this->polar.y = this->startPolar.y - diff.x;
            this->polar.x = glm::clamp(this->startPolar.x + diff.y, this->minLatitude, this->maxLatitude);
        }
        if (Input::TouchPinchingStarted()) {
            this->startDistance = this->distance;
        }
        if (Input::TouchPinching()) {
            float startDist = glm::length(glm::vec2(Input::TouchStartPosition(1) - Input::TouchStartPosition(0)));
            float curDist   = glm::length(glm::vec2(Input::TouchPosition(1) - Input::TouchPosition(0)));
            this->distance = glm::clamp(this->startDistance - (curDist - startDist) * 0.01f, this->minDist, this->maxDist);
        }
    }
}

//------------------------------------------------------------------------------
void TestInputApp::handleGamepadInput(int gamepadIndex) {
    const float rotatePerFrame = 0.025f;
    const float movePerFrame = 0.025f;
    const float deadZone = 0.2f;

    float leftStickHori = Input::GamepadAxisValue(gamepadIndex, GamepadAxis::LeftStickHori);
    float leftStickVert = Input::GamepadAxisValue(gamepadIndex, GamepadAxis::LeftStickVert);
    float rightStickHori = Input::GamepadAxisValue(gamepadIndex, GamepadAxis::RightStickHori);
    float rightStickVert = Input::GamepadAxisValue(gamepadIndex, GamepadAxis::RightStickVert);

    if ((leftStickHori > deadZone) || (leftStickHori < -deadZone)) {
        this->pointOfInterest -= leftStickHori * glm::vec3(this->invView[0]) * movePerFrame;
    }
    if ((leftStickVert > deadZone) || (leftStickVert < -deadZone)) {
        this->pointOfInterest += leftStickVert * glm::vec3(this->invView[1]) * movePerFrame;
    }
    if ((rightStickHori > deadZone) || (rightStickHori < -deadZone)) {
        this->polar.y -= rightStickHori * rotatePerFrame;
    }
    if ((rightStickVert > deadZone) || (rightStickVert < -deadZone)) {
        this->polar.x += rightStickVert * rotatePerFrame;
        this->polar.x = glm::clamp(this->polar.x, this->minLatitude, this->maxLatitude);
    }
}

//------------------------------------------------------------------------------
void
TestInputApp::drawCube() {
    Shader::vsParams vsParams;
    vsParams.mvp = this->proj * this->view;
    Gfx::ApplyDrawState(this->drawState);
    Gfx::ApplyUniformBlock(vsParams);
    Gfx::Draw();
}

//------------------------------------------------------------------------------
AppState::Code
TestInputApp::OnRunning() {
    // print input device status as debug text
    this->printMouseState();
    this->printKeyboardState();
    this->printTouchpadState();
    this->printSensorState();
    this->printGamepadState(selectedGamepadIndex);
    this->handleKeyboardInput();
    this->handleMouseInput();
    this->handleTouchInput();
    this->handleGamepadInput(selectedGamepadIndex);
    this->updateView();
    
    // draw frame
    Gfx::BeginPass(PassAction::Clear(this->getClearColor()));
    this->drawCube();
    Dbg::DrawTextBuffer();
    Gfx::EndPass();
    Gfx::CommitFrame();
    
    // continue running or quit?
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
TestInputApp::OnCleanup() {
    Input::Discard();
    Dbg::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}
