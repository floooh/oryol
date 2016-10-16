//------------------------------------------------------------------------------
//  VolumeTexture.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "shaders.h"

using namespace Oryol;

class VolumeTextureApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();

    DrawState drawState;
    int frameIndex = 0;
};
OryolMain(VolumeTextureApp);

//------------------------------------------------------------------------------
AppState::Code
VolumeTextureApp::OnInit() {
    auto gfxSetup = GfxSetup::Window(800, 600, "3D Texture Sample");
    Gfx::Setup(gfxSetup);

    // create a 64x64x64 RGBA volume texture
    const int dim = 64;
    const int mul = 256 / dim;
    uint8_t data[dim][dim][dim][4] = { };
    for (int z = 0; z < dim; z++) {
        for (int y = 0; y < dim; y++) {
            for (int x = 0; x < dim; x++) {
                data[z][y][x][0] = x*mul;
                data[z][y][x][1] = y*mul;
                data[z][y][x][2] = z*mul;
                data[z][y][x][3] = 0;
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
    Gfx::Discard();
    return App::OnCleanup();
}



