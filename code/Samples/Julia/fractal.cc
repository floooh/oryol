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
fractal::setup(int w, int h, const glm::vec4& rect_, Id fsq) {
    o_assert_dbg(!this->isValid());

    this->rect = rect_;
    this->frameIndex = 0;

    // generate a new resource label for our gfx resources
    this->label = Gfx::PushResourceLabel();

    // create the ping-pong render target that hold the fractal state
    auto rtSetup = TextureSetup::RenderTarget(w, h);
    rtSetup.ColorFormat = PixelFormat::RGBA32F;
    rtSetup.DepthFormat = PixelFormat::None;
    rtSetup.MinFilter = TextureFilterMode::Nearest;
    rtSetup.MagFilter = TextureFilterMode::Nearest;
    this->fractalTexture[0] = Gfx::CreateResource(rtSetup);
    this->fractalTexture[1] = Gfx::CreateResource(rtSetup);

    // create a color render target that holds the fractal state as color texture
    rtSetup.ColorFormat = PixelFormat::RGBA8;
    rtSetup.MinFilter = TextureFilterMode::Linear;
    rtSetup.MagFilter = TextureFilterMode::Linear;
    this->colorTexture = Gfx::CreateResource(rtSetup);

    // create draw state for updating the fractal state
    auto dss = DrawStateSetup::FromMeshAndShader(fsq, Gfx::CreateResource(Shaders::Julia::CreateSetup()));
    dss.RasterizerState.CullFaceEnabled = false;
    dss.DepthStencilState.DepthCmpFunc = CompareFunc::Always;
    dss.BlendState.ColorFormat = PixelFormat::RGBA32F;
    dss.BlendState.DepthFormat = PixelFormat::None;
    this->fractalDrawState = Gfx::CreateResource(dss);

    // create draw state to map fractal state into color texture
    dss.Shader = Gfx::CreateResource(Shaders::Color::CreateSetup());
    dss.BlendState.ColorFormat = PixelFormat::RGBA8;
    this->colorDrawState = Gfx::CreateResource(dss);

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
    this->colorDrawState.Invalidate();
}

//------------------------------------------------------------------------------
bool
fractal::isValid() const {
    return this->label.IsValid();
}

//------------------------------------------------------------------------------
void
fractal::update() {

    // first frame? clear fractal state to all zeros
    if (0 == this->frameIndex) {
        ClearState clearState = ClearState::ClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
        Gfx::ApplyRenderTarget(this->fractalTexture[0], clearState);
        Gfx::ApplyRenderTarget(this->fractalTexture[1], clearState);
    }

    // update frame index, and get the indices for write and read state
    const int32 writeIndex = this->frameIndex & 1;
    const int32 readIndex  = (this->frameIndex + 1) & 1;

    // render next fractal iteration
    ClearState noClearState = ClearState::ClearNone();
    Gfx::ApplyRenderTarget(this->fractalTexture[writeIndex], noClearState);
}

} // namespace Julia
