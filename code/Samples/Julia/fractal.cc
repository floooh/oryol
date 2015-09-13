//------------------------------------------------------------------------------
//  fractal.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "fractal.h"
#include "Gfx/Gfx.h"

using namespace Oryol;

namespace Julia {

//------------------------------------------------------------------------------
fractal::fractal() {
    // empty
}

//------------------------------------------------------------------------------
fractal::~fractal() {
    o_assert_dbg(!this->isValid());
}

//------------------------------------------------------------------------------
void
fractal::setup(int w, int h, const glm::vec4& rect_, const glm::vec2& pos_, Id fsq) {
    o_assert_dbg(!this->isValid());

    this->rect = rect_;
    this->pos = pos_;
    this->frameIndex = 0;

    // generate a new resource label for our gfx resources
    this->label = Gfx::PushResourceLabel();

    // the fractal-rendering shader
    Id fractalShader = Gfx::CreateResource(Shaders::Julia::Setup());

    // create the ping-pong render target that hold the fractal state
    auto rtSetup = TextureSetup::RenderTarget(w, h);
    rtSetup.ColorFormat = PixelFormat::RGBA32F;
    rtSetup.DepthFormat = PixelFormat::None;
    rtSetup.MinFilter = TextureFilterMode::Nearest;
    rtSetup.MagFilter = TextureFilterMode::Nearest;
    rtSetup.WrapU = TextureWrapMode::MirroredRepeat;
    rtSetup.WrapV = TextureWrapMode::MirroredRepeat;
    for (int i = 0; i < 2; i++) {
        this->fractalTexture[i] = Gfx::CreateResource(rtSetup);
        auto tbSetup = Shaders::Julia::FSTextures::Setup(fractalShader);
        tbSetup.Slot[Shaders::Julia::FSTextures::Texture] = this->fractalTexture[i];
        this->fractalTextureBlock[i] = Gfx::CreateResource(tbSetup);
    }

    // create a color render target that holds the fractal state as color texture
    rtSetup.ColorFormat = PixelFormat::RGBA8;
    rtSetup.MinFilter = TextureFilterMode::Linear;
    rtSetup.MagFilter = TextureFilterMode::Linear;
    this->colorTexture = Gfx::CreateResource(rtSetup);

    // create draw state for updating the fractal state
    auto dss = DrawStateSetup::FromMeshAndShader(fsq, fractalShader);
    dss.RasterizerState.CullFaceEnabled = false;
    dss.DepthStencilState.DepthCmpFunc = CompareFunc::Always;
    dss.BlendState.ColorFormat = PixelFormat::RGBA32F;
    dss.BlendState.DepthFormat = PixelFormat::None;
    this->fractalDrawState = Gfx::CreateResource(dss);

    // create draw state to map fractal state into color texture
    Id colorShader = Gfx::CreateResource(Shaders::Color::Setup());
    dss.Shader = colorShader;
    dss.BlendState.ColorFormat = PixelFormat::RGBA8;
    this->colorDrawState = Gfx::CreateResource(dss);

    for (int i = 0; i < 2; i++) {
        auto tbSetup = Shaders::Color::FSTextures::Setup(colorShader);
        tbSetup.Slot[Shaders::Color::FSTextures::Texture] = this->fractalTexture[i];
        this->colorTextureBlock[i] = Gfx::CreateResource(tbSetup);
    }

    Gfx::PopResourceLabel();
}

//------------------------------------------------------------------------------
void
fractal::discard() {
    o_assert_dbg(this->isValid());

    Gfx::DestroyResources(this->label);
    this->label.Invalidate();
    this->colorTexture.Invalidate();
    this->fractalDrawState.Invalidate();
    for (auto& tex : this->fractalTexture) {
        tex.Invalidate();
    }
    for (auto& tb : this->fractalTextureBlock) {
        tb.Invalidate();
    }
    this->colorDrawState.Invalidate();
    for (auto& tb : this->colorTextureBlock) {
        tb.Invalidate();
    }
}

//------------------------------------------------------------------------------
bool
fractal::isValid() const {
    return this->label.IsValid();
}

//------------------------------------------------------------------------------
void
fractal::update() {

    // increment frame and reset fractal state in first frame
    if (0 == this->frameIndex++) {
        const ClearState clearState = ClearState::ClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
        Gfx::ApplyRenderTarget(this->fractalTexture[0], clearState);
        Gfx::ApplyRenderTarget(this->fractalTexture[1], clearState);
    }

    // update frame index, and get the indices for write and read state
    const int32 writeIndex = this->frameIndex & 1;
    const int32 readIndex  = (this->frameIndex + 1) & 1;

    // render next fractal iteration
    this->fractalVSParams.Rect = this->rect;
    this->fractalFSParams.JuliaPos = this->pos;
    Gfx::ApplyRenderTarget(this->fractalTexture[writeIndex], ClearState::ClearNone());
    Gfx::ApplyDrawState(this->fractalDrawState);
    Gfx::ApplyUniformBlock(this->fractalVSParams);
    Gfx::ApplyUniformBlock(this->fractalFSParams);
    Gfx::ApplyTextureBlock(this->fractalTextureBlock[readIndex]);
    Gfx::Draw(0);

    // map current fractal state to color texture
    Gfx::ApplyRenderTarget(this->colorTexture, ClearState::ClearNone());
    Gfx::ApplyDrawState(this->colorDrawState);
    Gfx::ApplyTextureBlock(this->colorTextureBlock[writeIndex]);
    Gfx::Draw(0);

    if (this->frameIndex >= this->cycleCount) {
        this->frameIndex = 0;
    }
}

} // namespace Julia
