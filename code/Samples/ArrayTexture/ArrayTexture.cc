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

    PrimitiveGroup primGroup;
    Id pip;
    Bindings bind;
    int frameIndex = 0;
};
OryolMain(ArrayTextureApp);

//------------------------------------------------------------------------------
AppState::Code
ArrayTextureApp::OnInit() {
    Gfx::Setup(GfxDesc()
        .SetWidth(800)
        .SetHeight(512)
        .SetSampleCount(4)
        .SetTitle("Array Texture Sample")
        .SetHtmlTrackElementSize(true));
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
    this->bind.FSTexture[Shader::tex] = Gfx::CreateTexture(TextureDesc()
        .SetType(TextureType::TextureArray)
        .SetWidth(width)
        .SetHeight(height)
        .SetLayers(numLayers)
        .SetFormat(PixelFormat::RGBA8)
        .SetMinFilter(TextureFilterMode::Linear)
        .SetMagFilter(TextureFilterMode::Linear)
        .SetMipSize(0, 0, sizeof(data))
        .SetMipContent(0, 0, data));

    // build a cube mesh
    auto shape = ShapeBuilder()
        .Positions("in_pos", VertexFormat::Float3)
        .TexCoords("in_uv", VertexFormat::Float2)
        .Box(1.0f, 1.0f, 1.0f, 1)
        .Build();
    this->primGroup = shape.PrimitiveGroups[0];
    this->bind.VertexBuffers[0] = Gfx::CreateBuffer(shape.VertexBufferDesc);
    this->bind.IndexBuffer = Gfx::CreateBuffer(shape.IndexBufferDesc);

    // ...and a pipeline object to complete the DrawState
    this->pip = Gfx::CreatePipeline(PipelineDesc(shape.PipelineDesc)
        .SetShader(Gfx::CreateShader(Shader::Desc()))
        .SetDepthWriteEnabled(true)
        .SetDepthCmpFunc(CompareFunc::LessEqual)
        .SetSampleCount(Gfx::Desc().SampleCount));

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
    Gfx::BeginPass(PassAction().Clear(0.2f, 0.2f, 0.3f, 1.0f));
    Gfx::ApplyPipeline(this->pip);
    Gfx::ApplyBindings(this->bind);
    Gfx::ApplyUniforms(vsParams);
    Gfx::Draw(this->primGroup);
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

    glm::mat4 proj = glm::perspectiveFov(glm::radians(45.0f), float(Gfx::Width()), float(Gfx::Height()), 0.01f, 100.0f);
    const glm::vec3 cubePos(0.0f, 0.0f, -2.5f);
    float angleX = glm::radians(0.25f * this->frameIndex);
    float angleY = glm::radians(0.2f * this->frameIndex);
    glm::mat4 model = glm::translate(glm::mat4(), cubePos);
    model = glm::rotate(model, angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    vsParams.mvp = proj * model;

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
    uint8_t x = uint8_t((Gfx::DisplayAttrs().Width/16 - std::strlen(msg))/2);
    uint8_t y = uint8_t((Gfx::DisplayAttrs().Height/16)/2);
    Gfx::BeginPass(PassAction().Clear(0.5f, 0.0f, 0.0f, 1.0f));
    Dbg::TextScale(2.0f, 2.0f);
    Dbg::CursorPos(x, y);
    Dbg::Print(msg);
    Dbg::DrawTextBuffer();
    Gfx::EndPass();
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

