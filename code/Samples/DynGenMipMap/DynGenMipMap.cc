//------------------------------------------------------------------------------
//  DynGenMipMap.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Main.h"
#include "Gfx/Gfx.h"
#include "Assets/Gfx/ShapeBuilder.h"
#include "Dbg/Dbg.h"
#include "Input/Input.h"
#include "glm/gtc/matrix_transform.hpp"
#include "shaders.h"

using namespace Oryol;

class DynGenMipMap : public App {
public:
    /// on init frame method
    virtual AppState::Code OnInit();
    /// on running frame method
    virtual AppState::Code OnRunning();
    /// on cleanup frame method
    virtual AppState::Code OnCleanup();

    Shader::VSParams computeShaderParams();

    static const int TexWidth = 32;
    static const int TexHeight = 32;
    bool mipEnabled = true;
    Id mipTexture;
    Id noMipTexture;
    DrawState drawState;
    glm::mat4x4 proj;
    int frameIndex = 0;
};
OryolMain(DynGenMipMap);

//------------------------------------------------------------------------------
AppState::Code
DynGenMipMap::OnInit() {
    auto gfxSetup = GfxSetup::WindowMSAA4(512, 512, "DynGenMipMap");
    Gfx::Setup(gfxSetup);
    Dbg::Setup();
    Input::Setup();

    // create a dynamic texture with GenerateMipMaps flag, which will be overwritten each frame
    auto tex = TextureSetup::Empty2D(TexWidth, TexHeight, 1, PixelFormat::RGBA8, Usage::Stream);
    tex.Sampler.MinFilter = TextureFilterMode::LinearMipmapLinear;
    tex.Sampler.MagFilter = TextureFilterMode::Linear;
    this->noMipTexture = Gfx::CreateResource(tex);
    tex.NumMipMaps = 1 + int(std::floor(std::log2(std::max(TexWidth, TexHeight))));
    this->mipTexture = Gfx::CreateResource(tex);

    // create mesh, shader pipeline
    ShapeBuilder shapeBuilder;
    shapeBuilder.Layout
        .Add(VertexAttr::Position, VertexFormat::Float3)
        .Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    shapeBuilder.Box(1.0f, 1.0f, 1.0f, 1);
    this->drawState.Mesh[0] = Gfx::CreateResource(shapeBuilder.Build());
    Id shd = Gfx::CreateResource(Shader::Setup());
    auto pip = PipelineSetup::FromLayoutAndShader(shapeBuilder.Layout, shd);
    pip.DepthStencilState.DepthWriteEnabled = true;
    pip.DepthStencilState.DepthCmpFunc = CompareFunc::LessEqual;
    pip.RasterizerState.SampleCount = gfxSetup.SampleCount;
    this->drawState.Pipeline = Gfx::CreateResource(pip);

    // setup a projection matrix with the right aspect ratio
    const float fbWidth = (const float) Gfx::DisplayAttrs().FramebufferWidth;
    const float fbHeight = (const float) Gfx::DisplayAttrs().FramebufferHeight;
    this->proj = glm::perspectiveFov(glm::radians(45.0f), fbWidth, fbHeight, 0.01f, 100.0f);

    return App::OnInit();
}

//------------------------------------------------------------------------------
AppState::Code
DynGenMipMap::OnRunning() {
    this->frameIndex++;

    // select the mipmapped or non-mipmapped texture for updating and rendering
    this->drawState.FSTexture[0] = this->mipEnabled ? this->mipTexture : this->noMipTexture;

    uint32_t buf[TexWidth][TexHeight];
    const uint32_t c0 = this->frameIndex & 255;
    const uint32_t c1 = 255 - c0;
    for (int x = 0, evenOdd = 0; x < TexHeight; x++, evenOdd++) {
        for (int y = 0; y < TexWidth; y++, evenOdd++) {
            buf[y][x] = 0xFF000000 | ((evenOdd & 1) ? (c0|(c1<<8)) : 0);
        }
    }
    ImageDataAttrs imgAttrs;
    imgAttrs.NumFaces = 1;
    imgAttrs.NumMipMaps = 1;
    imgAttrs.Sizes[0][0] = sizeof(buf);
    Gfx::UpdateTexture(this->drawState.FSTexture[0], buf, imgAttrs);
    if (this->mipEnabled) {
        Gfx::GenerateMipmaps(this->drawState.FSTexture[0]);
    }

    // ...and draw
    Gfx::BeginPass();
    Gfx::ApplyDrawState(this->drawState);
    Gfx::ApplyUniformBlock(this->computeShaderParams());
    Gfx::Draw();

    // input handling and status text
    if ((Input::MouseAttached() && Input::MouseButtonDown(MouseButton::Left)) ||
        (Input::TouchpadAttached() && Input::TouchTapped())) {
        this->mipEnabled = !this->mipEnabled;
    }
    if (this->mipEnabled) {
        Dbg::Print("\n\n mipmapping enabled (click or tap to disable)");
    }
    else {
        Dbg::Print("\n\n mipmapping disabled (click or tap to enable)");
    }
    Dbg::DrawTextBuffer();
    Gfx::EndPass();
    Gfx::CommitFrame();

    return Gfx::QuitRequested() ? AppState::Cleanup : AppState::Running;
}

//------------------------------------------------------------------------------
AppState::Code
DynGenMipMap::OnCleanup() {
    Input::Discard();
    Dbg::Discard();
    Gfx::Discard();
    return App::OnCleanup();
}

//------------------------------------------------------------------------------
Shader::VSParams
DynGenMipMap::computeShaderParams() {
    Shader::VSParams vsParams;

    const glm::vec3 cubePos(0.0f, 0.0f, -((sin(this->frameIndex * 0.01) + 1.1f) * 20));
    float angleX = glm::radians(0.15f * this->frameIndex);
    float angleY = glm::radians(0.1f * this->frameIndex);
    glm::mat4 model = glm::translate(glm::mat4(), cubePos);
    model = glm::rotate(model, angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, angleY, glm::vec3(0.0f, 1.0f, 0.0f));
    vsParams.ModelViewProj = this->proj * model;

    return vsParams;
}

