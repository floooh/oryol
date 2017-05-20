//------------------------------------------------------------------------------
//  ArrayTexture.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "Dbg/Dbg.h"
#include "Assets/Gfx/ShapeBuilder.h"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"
#include <cstring>

using namespace Oryol;

class ArrayTextureApp : public App {
public:
    /// on init frame method
    virtual AppState::Code OnInit();
    /// on running frame method
    virtual AppState::Code OnRunning();
    /// on cleanup frame method
    virtual AppState::Code OnCleanup();

    AppState::Code notSupported();
    Shader::vsParams computeShaderParams();

    DrawState drawState;
    int frameIndex = 0;
    glm::mat4 proj;
};
OryolMain(ArrayTextureApp);

//------------------------------------------------------------------------------
AppState::Code
ArrayTextureApp::OnInit() {
    auto gfxSetup = GfxSetup::WindowMSAA4(800, 512, "Array Texture Sample");
    gfxSetup.DefaultPassAction = PassAction::Clear(glm::vec4(0.2f, 0.2f, 0.3f, 1.0f));
    Gfx::Setup(gfxSetup);
    Dbg::Setup();

    // if array textures are not supported, only show a warning
    if (!Gfx::QueryFeature(GfxFeature::TextureArray)) {
        return App::OnInit();
    }

    // create a 16x16 array texture with 3 layers and RGB checkboard pattern
    const int numLayers = 3;
    const int width = 16;
    const int height = 16;
    uint32_t data[numLayers][height][width];
    for (int layer = 0, evenOdd = 0; layer < numLayers; layer++, evenOdd++) {
        for (int y = 0; y < height; y++, evenOdd++) {
            for (int x = 0; x < width; x++, evenOdd++) {
                if (evenOdd & 1) {
                    switch (layer) {
                        case 0: data[layer][y][x] = 0xFF0000FF; break;
                        case 1: data[layer][y][x] = 0xFF00FF00; break;
                        case 2: data[layer][y][x] = 0xFFFF0000; break;
                    }
                }
                else {
                    data[layer][y][x] = 0;
                }
            }
        }
    }
    auto texSetup = TextureSetup::FromPixelDataArray(16, 16, numLayers, 1, PixelFormat::RGBA8);
    texSetup.Sampler.MinFilter = TextureFilterMode::Linear;
    texSetup.Sampler.MagFilter = TextureFilterMode::Linear;
    texSetup.ImageData.Sizes[0][0] = sizeof(data);
    this->drawState.FSTexture[Shader::tex] = Gfx::CreateResource(texSetup, data, sizeof(data));

    // build a cube mesh
    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout = {
        { VertexAttr::Position, VertexFormat::Float3 },
        { VertexAttr::TexCoord0, VertexFormat::Float2 }
    };
    shapeBuilder.Box(1.0f, 1.0f, 1.0f, 1);
    this->drawState.Mesh[0] = Gfx::CreateResource(shapeBuilder.Build());

    // ...and a pipeline object to complete the DrawState
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
ArrayTextureApp::OnRunning() {

    // if array textures not supported, just display a warning
    if (!Gfx::QueryFeature(GfxFeature::TextureArray)) {
        return this->notSupported();
    }

    // get model-view-proj matrix and animated uv offsets
    auto vsParams = this->computeShaderParams();

    // render texture cube
    Gfx::BeginPass();
    Gfx::ApplyDrawState(this->drawState);
    Gfx::ApplyUniformBlock(vsParams);
    Gfx::Draw();
    Gfx::EndPass();
    Gfx::CommitFrame();
    this->frameIndex++;
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}


//------------------------------------------------------------------------------
AppState::Code
ArrayTextureApp::OnCleanup() {
    Dbg::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
Shader::vsParams
ArrayTextureApp::computeShaderParams() {
    Shader::vsParams vsParams;

    float offset = float(this->frameIndex) * 0.001f;
    vsParams.uvOffset0 = glm::vec2(offset, -offset);
    vsParams.uvOffset1 = glm::vec2(-offset, offset);
    vsParams.uvOffset2 = glm::vec2(0.0f, 0.0f);

    const glm::vec3 cubePos(0.0f, 0.0f, -2.5f);
    float angleX = glm::radians(0.25f * this->frameIndex);
    float angleY = glm::radians(0.2f * this->frameIndex);
    glm::mat4 model = glm::translate(glm::mat4(), cubePos);
    model = glm::rotate(model, angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    vsParams.mvp = this->proj * model;

    return vsParams;
}

//------------------------------------------------------------------------------
AppState::Code
ArrayTextureApp::notSupported() {
    #if ORYOL_EMSCRIPTEN
    const char* msg = "This demo needs WebGL2\n";
    #else
    const char* msg = "This demo needs array texture support\n";
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

