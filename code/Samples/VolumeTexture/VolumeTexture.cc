//------------------------------------------------------------------------------
//  VolumeTexture.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "Dbg/Dbg.h"
#include "Assets/Gfx/ShapeBuilder.h"
#include "shaders.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/noise.hpp"
#include <cstring>

using namespace Oryol;

class VolumeTextureApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();

    AppState::Code notSupported();    // render a warning if 3D textures not supported by platform
    void computeShaderParams();

    DrawState drawState;
    Shader::vsParams vsParams;
    int frameIndex = 0;
    glm::mat4 proj;
};
OryolMain(VolumeTextureApp);

//------------------------------------------------------------------------------
AppState::Code
VolumeTextureApp::OnInit() {
    auto gfxSetup = GfxSetup::WindowMSAA4(800, 600, "3D Texture Sample");
    gfxSetup.DefaultPassAction = PassAction::Clear(glm::vec4(0.25f, 0.25f, 0.25f, 1.0f));
    Gfx::Setup(gfxSetup);
    Dbg::Setup();

    // if 3D textures not supported show a warning later during rendering
    if (!Gfx::QueryFeature(GfxFeature::Texture3D)) {
        return App::OnInit();
    }

    // create a RGBA volume texture
    glm::vec3 p(0.0f, 0.0f, 0.0f);
    const int dim = 32;
    static uint8_t data[dim][dim][dim][4] = { };
    for (int z = 0; z < dim; z++) {
        p.y = 0.0f;
        for (int y = 0; y < dim; y++) {
            p.x = 0.0f;
            for (int x = 0; x < dim; x++) {
                data[x][y][z][0] = uint8_t(glm::simplex(p * 1.0f) * 255.0f);
                data[x][y][z][1] = uint8_t(glm::simplex(p * 1.2f) * 255.0f);
                data[x][y][z][2] = uint8_t(glm::simplex(p * 1.4f) * 255.0f);
                data[x][y][z][3] = 0;
                p.x += 1.0f / dim;
            }
            p.y += 1.0f / dim;
        }
        p.z += 1.0f / dim;
    }
    auto texSetup = TextureSetup::FromPixelData3D(dim, dim, dim, 1, PixelFormat::RGBA8);
    texSetup.Sampler.MinFilter = TextureFilterMode::Linear;
    texSetup.Sampler.MagFilter = TextureFilterMode::Linear;
    texSetup.ImageData.Sizes[0][0] = sizeof(data);
    this->drawState.FSTexture[Shader::tex] = Gfx::CreateResource(texSetup, data, sizeof(data));

    // create a cube which will be the hull geometry for raycasting through the 3D texture
    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout = { { VertexAttr::Position, VertexFormat::Float3 } };
    shapeBuilder.Box(1.0f, 1.0f, 1.0f, 1);
    this->drawState.Mesh[0] = Gfx::CreateResource(shapeBuilder.Build());

    // pipeline state for rendering the 3D-textures cube
    Id shd = Gfx::CreateResource(Shader::Setup());
    auto pipSetup = PipelineSetup::FromLayoutAndShader(shapeBuilder.Layout, shd);
    pipSetup.DepthStencilState.DepthWriteEnabled = true;
    pipSetup.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    pipSetup.RasterizerState.SampleCount = gfxSetup.SampleCount;
    this->drawState.Pipeline = Gfx::CreateResource(pipSetup);

    // setup a projection matrix with the right aspect ratio
    const float fbWidth = (const float) Gfx::DisplayAttrs().FramebufferWidth;
    const float fbHeight = (const float) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
VolumeTextureApp::OnRunning() {

    // if 3D textures are not supported, just render a warning
    if (!Gfx::QueryFeature(GfxFeature::Texture3D)) {
        return this->notSupported();
    }

    this->computeShaderParams();

    // render the rotating cube
    Gfx::BeginPass();
    Gfx::ApplyDrawState(this->drawState);
    Gfx::ApplyUniformBlock(this->vsParams);
    Gfx::Draw();
    Gfx::EndPass();
    Gfx::CommitFrame();
    this->frameIndex++;
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
VolumeTextureApp::OnCleanup() {
    Dbg::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
void
VolumeTextureApp::computeShaderParams() {
    const glm::vec4 eyePos(0.0f, 0.0f, 0.0f, 1.0f);
    const glm::vec3 pos(0.0f, 0.0f, -2.0f);

    float angleX = glm::radians(0.2f * this->frameIndex);
    float angleY = glm::radians(0.1f * this->frameIndex);
    glm::mat4 model = glm::translate(glm::mat4(), pos);
    model = glm::rotate(model, angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 invModel = glm::inverse(model);
    this->vsParams.mvp = this->proj * model;
    this->vsParams.modelEyePos = invModel * eyePos;
}

//------------------------------------------------------------------------------
AppState::Code
VolumeTextureApp::notSupported() {
    #if ORYOL_EMSCRIPTEN
    const char* msg = "This demo needs WebGL2\n";
    #else
    const char* msg = "This demo needs 3D texture support\n";
    #endif
    uint8_t x = uint8_t((Gfx::DisplayAttrs().FramebufferWidth/16 - std::strlen(msg))/2);
    uint8_t y = uint8_t(Gfx::DisplayAttrs().FramebufferHeight/16/2);
    Gfx::BeginPass(PassAction::Clear(glm::vec4(0.5f, 0.0f, 0.0f, 1.0f)));
    Dbg::TextScale(2.0f, 2.0f);
    Dbg::CursorPos(x, y);
    Dbg::Print(msg);
    Dbg::DrawTextBuffer();
    Gfx::EndPass();
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}


