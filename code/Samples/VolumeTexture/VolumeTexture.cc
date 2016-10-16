//------------------------------------------------------------------------------
//  VolumeTexture.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "Dbg/Dbg.h"
#include "shaders.h"

using namespace Oryol;

class VolumeTextureApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();

    AppState::Code notSupported();    // render a warning if 3D textures not supported by platform

    DrawState drawState;
    int frameIndex = 0;
};
OryolMain(VolumeTextureApp);

//------------------------------------------------------------------------------
AppState::Code
VolumeTextureApp::OnInit() {
    auto gfxSetup = GfxSetup::Window(800, 600, "3D Texture Sample");
    Gfx::Setup(gfxSetup);
    Dbg::Setup();

    // if 3D textures not supported show a warning later during rendering
    if (!Gfx::QueryFeature(GfxFeature::Texture3D)) {
        return App::OnInit();
    }

    // create a RGBA volume texture
    const int dim = 16;
    const int mul = 256 / dim;
    uint8_t data[dim][dim][dim][4] = { };
    for (int z = 0; z < dim; z++) {
        for (int y = 0; y < dim; y++) {
            for (int x = 0; x < dim; x++) {
                data[z][y][x][0] = x*mul;
                data[z][y][x][1] = y*mul;
                data[z][y][x][2] = z*mul;
                data[z][y][x][3] = 255;
            }
        }
    }
    auto texSetup = TextureSetup::FromPixelData3D(dim, dim, dim, 1, PixelFormat::RGBA8);
    texSetup.Sampler.MinFilter = TextureFilterMode::Linear;
    texSetup.Sampler.MagFilter = TextureFilterMode::Linear;
    this->drawState.FSTexture[Textures::Texture] = Gfx::CreateResource(texSetup, data, sizeof(data));
    auto fsqSetup = MeshSetup::FullScreenQuad();
    this->drawState.Mesh[0] = Gfx::CreateResource(fsqSetup);

    Id shd = Gfx::CreateResource(Shader::Setup());
    auto pipSetup = PipelineSetup::FromLayoutAndShader(fsqSetup.Layout, shd);
    pipSetup.DepthStencilState.DepthWriteEnabled = false;
    pipSetup.DepthStencilState.DepthCmpFunc = CompareFunc::Always;
    pipSetup.RasterizerState.SampleCount = gfxSetup.SampleCount;
    this->drawState.Pipeline = Gfx::CreateResource(pipSetup);

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
VolumeTextureApp::OnRunning() {

    // if 3D textures are not supported, just render a warning
    if (!Gfx::QueryFeature(GfxFeature::Texture3D)) {
        return this->notSupported();
    }

    Shader::VSParams vsParams;
    float d = float(this->frameIndex % 256) / 256.0f;
    vsParams.TexOffset = glm::vec3(d, d, d);

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
VolumeTextureApp::OnCleanup() {
    Dbg::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
AppState::Code
VolumeTextureApp::notSupported() {
    #if ORYOL_EMSCRIPTEN
    const char* msg = "This demo needs WebGL2\n";
    #else
    const char* msg = "This demo needs 3D texture support\n";
    #endif
    uint8_t x = (Gfx::DisplayAttrs().FramebufferWidth/16 - strlen(msg))/2;
    uint8_t y = Gfx::DisplayAttrs().FramebufferHeight/16/2;
    Gfx::BeginPass(PassState(glm::vec4(0.5f, 0.0f, 0.0f, 1.0f)));
    Dbg::SetTextScale(glm::vec2(2.0f, 2.0f));
    Dbg::CursorPos(x, y);
    Dbg::Print(msg);
    Dbg::DrawTextBuffer();
    Gfx::EndPass();
    Gfx::CommitFrame();
    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}


