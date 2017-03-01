//------------------------------------------------------------------------------
//  TestInput.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "Assets/Gfx/ShapeBuilder.h"
#include "Dbg/Dbg.h"
#include "Input/Input.h"
//#include "Input/Core/inputMgrBase.h"
#include "Core/String/StringConverter.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/polar_coordinates.hpp"
#include "shaders.h"

using namespace Oryol;

class TestInputApp : public App {
public:
    AppState::Code OnRunning();
    AppState::Code OnInit();
    AppState::Code OnCleanup();
    
    /// virtual onFrame method to be overwritten by subclass
    //virtual void onFrame();

private:
    void testMouseButton(MouseButton::Code btn, const char* name);
    void testKey(Key::Code key, const char* name);
    void printMouseState();
    void printKeyboardState();
    void printTouchpadState();
    void printSensorState();
    void printGamepadState(int gamepadIndex);
    void printGamepadsState();
    glm::vec4 getClearColor();
    void updateView();
    void reset();
    void drawCube();
    void handleKeyboardInput();
    void handleMouseInput();
    void handleTouchInput();
    
    const glm::vec4 downColor{1.0f, 0.0f, 0.0f, 1.0f};
    const glm::vec4 upColor{0.0f, 0.0f, 1.0f, 1.0f};
    const glm::vec4 pressedColor{0.0f, 1.0f, 0.0f, 1.0f};
    const glm::vec4 defaultColor{1.0f, 1.0f, 1.0f, 0.5f};
    float minLatitude;
    float maxLatitude;
    float minDist;
    float maxDist;

    DrawState drawState;
    ClearState clearState;
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
        Dbg::SetTextScale(glm::vec2(2.0f, 2.0f));
    }

    InputSetup setup;
    setup.PollingRequired = true;

    Input::Setup(setup);
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
    shapeBuilder.Layout
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::Normal, VertexFormat::Byte4N);
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
//void
//TestInputApp::onFrame()
//{
//	//Input::Update();
//}

//------------------------------------------------------------------------------
void
TestInputApp::updateView() {
    // first rotate around global Y axis, then rotate around global X axis (transformed to local space)
    glm::vec3 eucl = glm::euclidean(this->polar) * distance;
    this->view = glm::lookAt(eucl + this->pointOfInterest, this->pointOfInterest, glm::vec3(0.0f, 1.0f, 0.0f));
    this->invView = glm::inverse(this->view);

//    Input::Update();
}

//------------------------------------------------------------------------------
void
TestInputApp::testMouseButton(MouseButton::Code btn, const char* name) {
    glm::vec4 color;
    if (Input::MouseButtonDown(btn)) color = this->downColor;
    else if (Input::MouseButtonUp(btn)) color = this->upColor;
    else if (Input::MouseButtonPressed(btn)) color = this->pressedColor;
    else color = this->defaultColor;
    Dbg::TextColor(color);
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
        Dbg::TextColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        Dbg::Print("\n MOUSE STATUS (LMB for pointerlock):\n\n\r");
        
        this->testMouseButton(MouseButton::Left, "LMB");
        this->testMouseButton(MouseButton::Middle, "MMB");
        this->testMouseButton(MouseButton::Right, "RMB");
        Dbg::TextColor(this->pointerLock ? this->pressedColor : this->defaultColor);
        Dbg::PrintF(" POINTERLOCK");

        Dbg::TextColor(glm::vec4(1.0f));
        Dbg::PrintF("\n\r pos: %.3f %.3f\n\r mov: %.3f %.3f\n\r scroll: %.3f %.3f",
                    Input::MousePosition().x, Input::MousePosition().y,
                    Input::MouseMovement().x, Input::MouseMovement().y,
                    Input::MouseScroll().x, Input::MouseScroll().y);
    }
    else {
        Dbg::TextColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        Dbg::Print("\n MOUSE NOT ATTACHED");
    }
}

//------------------------------------------------------------------------------
void
TestInputApp::printKeyboardState() {
    if (Input::KeyboardAttached()) {
        Dbg::TextColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
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
        Dbg::TextColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        Dbg::Print("\n\n\r KEYBOARD NOT ATTACHED");
    };
}

//------------------------------------------------------------------------------
void
TestInputApp::printTouchpadState() {
    if (Input::TouchpadAttached()) {
        Dbg::TextColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
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
        Dbg::TextColor(glm::vec4(1.0f));
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
        Dbg::TextColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        Dbg::Print("\n\n\r TOUCHPAD NOT ATTACHED");
    }
}

//------------------------------------------------------------------------------
void
TestInputApp::printSensorState() {
    if (Input::SensorsAttached()) {
        Dbg::TextColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        Dbg::Print("\n\n\r SENSOR STATUS:\n\n\r");
        Dbg::TextColor(glm::vec4(1.0f));
        const glm::vec3& acc = Input::SensorAcceleration();
        const glm::vec3& ypr = Input::SensorYawPitchRoll();
        Dbg::PrintF(" acceleration: %.3f %.3f %.3f\n\r", acc.x, acc.y, acc.z);
        Dbg::PrintF(" yaw: %.3f, pitch: %.3f, roll: %.3f\n\r",
                    glm::degrees(ypr.x), glm::degrees(ypr.y), glm::degrees(ypr.z));
    }
    else {
        Dbg::TextColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        Dbg::Print("\n\n\r SENSORS NOT ATTACHED");
    }
}
void
TestInputApp::printGamepadState(int gamepadIndex)
{
	//assert(gamepadIndex >= 0 && gamepadIndex < GamePad::NUM)

	if (Input::GamepadAttached(gamepadIndex))
	{
        Dbg::TextColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        Dbg::PrintF("\n\n\r GAMEPAD %d STATUS:\n\n\r", gamepadIndex);
        Dbg::TextColor(glm::vec4(1.0f));
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadGizmo::A) ? " A" : "  ");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadGizmo::B) ? " B" : "  ");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadGizmo::X) ? " X" : "  ");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadGizmo::Y) ? " Y\n\r" : "\n\r");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadGizmo::Start) ? " Start" : "      ");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadGizmo::Back) ? " Back\n\r" : "\n\r");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadGizmo::LeftBumper) ? " LeftBumper\t" : "            ");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadGizmo::RightBumper) ? " RightBumper\n\r" : "\n\r");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadGizmo::DPadLeft) ? " DPadLeft" : "         ");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadGizmo::DPadRight) ? " DPadRight" : "          ");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadGizmo::DPadUp) ? " DPadUp" : "       ");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadGizmo::DPadDown) ? " DPadDown\n\r" : "        \n\r");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadGizmo::LeftTrigger) ? " LeftTrigger" : "            ");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadGizmo::RightTrigger) ? " RightTrigger\n\r" : "           \n\r");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadGizmo::LeftStick) ? " LeftStick  " : "            ");
        Dbg::PrintF(Input::GamepadButtonPressed(gamepadIndex, GamepadGizmo::RightStick) ? " RightStick\n\r" : "          \n\r");
        Dbg::PrintF(" LeftStick.x %4.4f", Input::GamepadStickPos(gamepadIndex, GamepadGizmo::LeftStick).x);
        Dbg::PrintF(" LeftStick.y %4.4f\n\r", Input::GamepadStickPos(gamepadIndex, GamepadGizmo::LeftStick).y);
        Dbg::PrintF(" RightStick.x %4.4f", Input::GamepadStickPos(gamepadIndex, GamepadGizmo::RightStick).x);
        Dbg::PrintF(" RightStick.y %4.4f\n\r", Input::GamepadStickPos(gamepadIndex, GamepadGizmo::RightStick).y);
        Dbg::PrintF(" LeftTrigger %4.4f", Input::GamepadStickPos(gamepadIndex, GamepadGizmo::LeftTrigger).x);
        Dbg::PrintF(" RightTrigger %4.4f\n\r", Input::GamepadStickPos(gamepadIndex, GamepadGizmo::RightTrigger).x);

//        Dbg::PrintF(" DPadUp.x %4.4f", Input::GamepadStickPos(gamepadIndex, GamepadGizmo::DPadUp).x);
	}
//	else
//	{
//        Dbg::TextColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
//        Dbg::Print("\n\n\r GAMEPAD NOT ATTACHED");
//	}
}
void
TestInputApp::printGamepadsState()
{
	// FIXME
	int MaxNumGamepads = 4;

	for(int i = 0; i < MaxNumGamepads; ++i)
	{
		printGamepadState(i);
	}
}

//------------------------------------------------------------------------------
glm::vec4
TestInputApp::getClearColor() {
    glm::vec4 clearColor(0.25f, 0.25f, 0.25f, 1.0f);
    if (Input::TouchTapped()) {
        clearColor = this->downColor;
    }
    if (Input::TouchDoubleTapped()) {
        clearColor = this->upColor;
    }
    return clearColor;
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
void
TestInputApp::drawCube() {
    Shader::VSParams vsParams;
    vsParams.ModelViewProjection = this->proj * this->view;
    Gfx::ApplyDrawState(this->drawState);
    Gfx::ApplyUniformBlock(vsParams);
    Gfx::Draw();
}

//------------------------------------------------------------------------------
AppState::Code
TestInputApp::OnRunning() {

	Input::Update();

    // print input device status as debug text
    this->printMouseState();
    this->printKeyboardState();
    this->printTouchpadState();
    this->printSensorState();
    this->printGamepadsState();
    this->handleKeyboardInput();
    this->handleMouseInput();
    this->handleTouchInput();
    this->updateView();
    
    // draw frame
    this->clearState.Color = this->getClearColor();
    Gfx::ApplyDefaultRenderTarget(this->clearState);
    this->drawCube();
    Dbg::DrawTextBuffer();
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
