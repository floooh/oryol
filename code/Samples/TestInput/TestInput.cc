//------------------------------------------------------------------------------
//  TestInput.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/App.h"
#include "Gfx/Gfx.h"
#include "Assets/Gfx/ShapeBuilder.h"
#include "Dbg/Dbg.h"
#include "Input/Input.h"
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
    
private:
    void testMouseButton(const Mouse& mouse, Mouse::Button btn, const char* name) const;
    void testKey(const Keyboard& kbd, Key::Code key, const char* name) const;
    void printMouseState(const Mouse& mouse) const;
    void printKeyboardState(const Keyboard& kbd) const;
    void printTouchpadState(const Touchpad& touchpad) const;
    void printSensorState(const Sensors& sensors) const;
    glm::vec4 getClearColor(const Touchpad& touchpad) const;
    void updateView();
    void reset();
    void drawCube() const;
    void handleKeyboardInput(const Keyboard& kbd);
    void handleMouseInput(const Mouse& mouse);
    void handleTouchInput(const Touchpad& touchpad);
    
    const glm::vec4 downColor{1.0f, 0.0f, 0.0f, 1.0f};
    const glm::vec4 upColor{0.0f, 0.0f, 1.0f, 1.0f};
    const glm::vec4 pressedColor{0.0f, 1.0f, 0.0f, 1.0f};
    const glm::vec4 defaultColor{1.0f, 1.0f, 1.0f, 0.5f};
    float32 minLatitude;
    float32 maxLatitude;
    float32 minDist;
    float32 maxDist;
    
    Id drawState;
    glm::vec2 startPolar;
    glm::vec2 polar;
    float32 distance = 6.0f;
    float32 startDistance = 6.0f;
    glm::vec2 startMousePos;
    glm::vec3 pointOfInterest;
    glm::mat4 proj;
    glm::mat4 view;
    glm::mat4 invView;
};
OryolMain(TestInputApp);

//------------------------------------------------------------------------------
AppState::Code
TestInputApp::OnInit() {
    Gfx::Setup(GfxSetup::Window(800, 400, "Oryol Input Test Sample"));
    Dbg::Setup();
    if (Gfx::DisplayAttrs().WindowWidth > 800) {
        Dbg::SetTextScale(glm::vec2(2.0f, 2.0f));
    }
    Input::Setup();
    
    // create a 3D cube
    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::Normal, VertexFormat::Byte4N);
    shapeBuilder.Box(1.0f, 1.0f, 1.0f, 1).Build();
    Id mesh = Gfx::CreateResource(shapeBuilder.Result());
    Id prog = Gfx::CreateResource(Shaders::Main::CreateSetup());
    auto dss = DrawStateSetup::FromMeshAndProg(mesh, prog);
    dss.DepthStencilState.DepthWriteEnabled = true;
    dss.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    dss.RasterizerState.CullFaceEnabled = true;
    this->drawState = Gfx::CreateResource(dss);

    // setup transform matrices
    const float32 fbWidth = (const float32) Gfx::DisplayAttrs().FramebufferWidth;
    const float32 fbHeight = (const float32) Gfx::DisplayAttrs().FramebufferHeight;
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
TestInputApp::testMouseButton(const Mouse& mouse, Mouse::Button btn, const char* name) const {
    glm::vec4 color;
    if (mouse.ButtonDown(btn)) color = this->downColor;
    else if (mouse.ButtonUp(btn)) color = this->upColor;
    else if (mouse.ButtonPressed(btn)) color = this->pressedColor;
    else color = this->defaultColor;
    Dbg::TextColor(color);
    Dbg::PrintF(" %s", name);
}

//------------------------------------------------------------------------------
void
TestInputApp::testKey(const Keyboard& kbd, Key::Code key, const char* name) const {
    glm::vec4 color;
    if (kbd.KeyDown(key)) {
        Dbg::TextColor(this->downColor);
        Dbg::PrintF(" %s", Key::ToString(key));
    }
    else if (kbd.KeyUp(key)) {
        Dbg::TextColor(this->upColor);
        Dbg::PrintF(" %s", Key::ToString(key));
    }
    else if (kbd.KeyPressed(key)) {
        Dbg::TextColor(this->pressedColor);
        Dbg::PrintF(" %s", Key::ToString(key));
    }
}

//------------------------------------------------------------------------------
void
TestInputApp::printMouseState(const Mouse& mouse) const {
    if (mouse.Attached) {
        Dbg::TextColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        Dbg::Print("\n MOUSE STATUS (Enter for pointerlock):\n\n\r");
        
        this->testMouseButton(mouse, Mouse::LMB, "LMB");
        this->testMouseButton(mouse, Mouse::MMB, "MMB");
        this->testMouseButton(mouse, Mouse::RMB, "RMB");
        
        Dbg::TextColor(glm::vec4(1.0f));
        Dbg::PrintF("\n\r pos: %.3f %.3f\n\r mov: %.3f %.3f\n\r scroll: %.3f %.3f",
                    mouse.Position.x, mouse.Position.y,
                    mouse.Movement.x, mouse.Movement.y,
                    mouse.Scroll.x, mouse.Scroll.y);
    }
    else {
        Dbg::TextColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        Dbg::Print("\n MOUSE NOT ATTACHED");
    }
}

//------------------------------------------------------------------------------
void
TestInputApp::printKeyboardState(const Keyboard& kbd) const {
    if (kbd.Attached) {
        Dbg::TextColor(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
        Dbg::Print("\n\n\r KEYBOARD STATUS (Enter to capture text):\n\n\r");
        if (kbd.KeyDown(Key::Enter)) {
            if (CursorMode::Disabled != Input::GetCursorMode()) {
                Input::SetCursorMode(CursorMode::Disabled);
            }
            else {
                Input::SetCursorMode(CursorMode::Normal);
            }
            if (!kbd.IsCapturingText()) {
                Input::BeginCaptureText();
            }
            else {
                Input::EndCaptureText();
            }
        }
        if (kbd.IsCapturingText()) {
            Dbg::Print(" capturing: ");
            String str = StringConverter::WideToUTF8(kbd.CapturedText());
            Dbg::PrintF("%s\n\r", str.AsCStr());
        }
        else {
            Dbg::Print(" keys: ");
            for (int32 key = 0; key < Key::NumKeys; key++) {
                this->testKey(kbd, (Key::Code)key, Key::ToString((Key::Code)key));
            }
        }
    }
    else {
        Dbg::TextColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        Dbg::Print("\n\n\r KEYBOARD NOT ATTACHED");
    };
}

//------------------------------------------------------------------------------
void
TestInputApp::printTouchpadState(const Touchpad& touchpad) const {
    if (touchpad.Attached) {
        Dbg::TextColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        Dbg::Print("\n\n\r TOUCHPAD STATUS:\n\n\r");
        if (touchpad.Tapped) {
            Dbg::TextColor(this->pressedColor);
        }
        else {
            Dbg::TextColor(this->defaultColor);
        }
        Dbg::Print(" TAPPED ");
        if (touchpad.DoubleTapped) {
            Dbg::TextColor(this->pressedColor);
        }
        else {
            Dbg::TextColor(this->defaultColor);
        }
        Dbg::Print("DOUBLETAPPED ");
        if (touchpad.Panning) {
            Dbg::TextColor(this->pressedColor);
        }
        else {
            Dbg::TextColor(this->defaultColor);
        }
        Dbg::Print("PANNING ");
        if (touchpad.Pinching) {
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
                    touchpad.Position(0).x, touchpad.Position(0).y,
                    touchpad.Movement(0).x, touchpad.Movement(0).y,
                    touchpad.StartPosition(0).x, touchpad.StartPosition(0).y,
                    touchpad.Position(1).x, touchpad.Position(1).y,
                    touchpad.Movement(1).x, touchpad.Movement(1).y,
                    touchpad.StartPosition(1).x, touchpad.StartPosition(1).y);
    }
    else {
        Dbg::TextColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        Dbg::Print("\n\n\r TOUCHPAD NOT ATTACHED");
    }
}

//------------------------------------------------------------------------------
void
TestInputApp::printSensorState(const Sensors& sensors) const {
    if (sensors.Attached) {
        Dbg::TextColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        Dbg::Print("\n\n\r SENSOR STATUS:\n\n\r");
        Dbg::TextColor(glm::vec4(1.0f));
        Dbg::PrintF(" acceleration: %.3f %.3f %.3f\n\r",
                    sensors.Acceleration.x,
                    sensors.Acceleration.y,
                    sensors.Acceleration.z);
        Dbg::PrintF(" yaw: %.3f, pitch: %.3f, roll: %.3f\n\r",
                    glm::degrees(sensors.Yaw),
                    glm::degrees(sensors.Pitch),
                    glm::degrees(sensors.Roll));
    }
    else {
        Dbg::TextColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        Dbg::Print("\n\n\r SENSORS NOT ATTACHED");
    }
}

//------------------------------------------------------------------------------
glm::vec4
TestInputApp::getClearColor(const Touchpad& touchpad) const {
    glm::vec4 clearColor(0.25f);
    if (touchpad.Tapped) {
        clearColor = this->downColor;
    }
    if (touchpad.DoubleTapped) {
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
TestInputApp::handleKeyboardInput(const Keyboard& kbd) {
    if (kbd.Attached) {

        if (kbd.KeyDown(Key::Space)) {
            this->reset();
        }

        static const float32 rotatePerFrame = 0.025f;
        static const float32 movePerFrame = 0.025f;
        
        if (kbd.KeyPressed(Key::LeftShift)) {
            // rotate cube
            if (kbd.KeyPressed(Key::Left)) {
                this->polar.y -= rotatePerFrame;
            }
            if (kbd.KeyPressed(Key::Right)) {
                this->polar.y += rotatePerFrame;
            }
            if (kbd.KeyPressed(Key::Up)) {
                this->polar.x = glm::clamp(this->polar.x - rotatePerFrame, this->minLatitude, this->maxLatitude);
            }
            if (kbd.KeyPressed(Key::Down)) {
                this->polar.x = glm::clamp(this->polar.x + rotatePerFrame, this->minLatitude, this->maxLatitude);
            }
        }
        else {
            // move cube
            if (kbd.KeyPressed(Key::Left)) {
                this->pointOfInterest += glm::vec3(this->invView[0]) * movePerFrame;
            }
            if (kbd.KeyPressed(Key::Right)) {
                this->pointOfInterest -= glm::vec3(this->invView[0]) * movePerFrame;
            }
            if (kbd.KeyPressed(Key::Up)) {
                this->pointOfInterest -= glm::vec3(this->invView[1]) * movePerFrame;
            }
            if (kbd.KeyPressed(Key::Down)) {
                this->pointOfInterest += glm::vec3(this->invView[1]) * movePerFrame;
            }
        }
    }
}

//------------------------------------------------------------------------------
void
TestInputApp::handleMouseInput(const Mouse& mouse) {
    if (mouse.Attached) {
        if (mouse.ButtonPressed(Mouse::LMB)) {
            this->polar.y -= mouse.Movement.x * 0.01f;
            this->polar.x = glm::clamp(this->polar.x + mouse.Movement.y * 0.01f, this->minLatitude, this->maxLatitude);
        }
        this->distance = glm::clamp(this->distance + mouse.Scroll.y * 0.1f, this->minDist, this->maxDist);
    }
}

//------------------------------------------------------------------------------
void
TestInputApp::handleTouchInput(const Touchpad& touchpad) {
    if (touchpad.Attached) {
        if (touchpad.DoubleTapped) {
            this->reset();
        }
        
        if (touchpad.PanningStarted) {
            this->startPolar = this->polar;
        }
        if (touchpad.Panning) {
            glm::vec2 diff = (touchpad.Position(0) - touchpad.StartPosition(0)) * 0.01f;
            this->polar.y = this->startPolar.y - diff.x;
            this->polar.x = glm::clamp(this->startPolar.x + diff.y, this->minLatitude, this->maxLatitude);
        }
        if (touchpad.PinchingStarted) {
            this->startDistance = this->distance;
        }
        if (touchpad.Pinching) {
            float32 startDist = glm::length(glm::vec2(touchpad.StartPosition(1) - touchpad.StartPosition(0)));
            float32 curDist   = glm::length(glm::vec2(touchpad.Position(1) - touchpad.Position(0)));
            this->distance = glm::clamp(this->startDistance - (curDist - startDist) * 0.01f, this->minDist, this->maxDist);
        }
    }
}

//------------------------------------------------------------------------------
void
TestInputApp::drawCube() const {
    glm::mat4 mvp = this->proj * this->view;
    Gfx::ApplyDrawState(this->drawState);
    Gfx::ApplyVariable(Shaders::Main::ModelViewProjection, mvp);
    Gfx::Draw(0);
}

//------------------------------------------------------------------------------
AppState::Code
TestInputApp::OnRunning() {

    // print input device status as debug text
    const Keyboard& kbd = Input::Keyboard();
    const Mouse& mouse = Input::Mouse();
    const Touchpad& touchpad = Input::Touchpad();
    const Sensors& sensors = Input::Sensors();
    this->printMouseState(mouse);
    this->printKeyboardState(kbd);
    this->printTouchpadState(touchpad);
    this->printSensorState(sensors);
    this->handleKeyboardInput(kbd);
    this->handleMouseInput(mouse);
    this->handleTouchInput(touchpad);
    this->updateView();
    
    // draw frame
    Gfx::ApplyDefaultRenderTarget();
    Gfx::Clear(ClearTarget::All, this->getClearColor(touchpad), 1.0f, 0);
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
