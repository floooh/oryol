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
fractal::setup(int w, int h, const glm::vec4& rect_, const glm::vec2& pos_, Id fsqMesh, const VertexLayout& fsqLayout) {
    o_assert_dbg(!this->isValid());

    this->rect = rect_;
    this->pos = pos_;
    this->frameIndex = 0;

    // generate a new resource label for our gfx resources
    this->label = Gfx::PushResourceLabel();

    // the fractal-rendering shader
    Id fractalShader = Gfx::CreateResource(JuliaShader::Setup());

    // create the ping-pong render target that hold the fractal state
    auto rtSetup = TextureSetup::RenderTarget(w, h);
    rtSetup.ColorFormat = PixelFormat::RGBA32F;
    rtSetup.DepthFormat = PixelFormat::None;
    rtSetup.ClearHint = ClearState::ClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
    rtSetup.Sampler.MinFilter = TextureFilterMode::Nearest;
    rtSetup.Sampler.MagFilter = TextureFilterMode::Nearest;
    rtSetup.Sampler.WrapU = TextureWrapMode::MirroredRepeat;
    rtSetup.Sampler.WrapV = TextureWrapMode::MirroredRepeat;
    for (int i = 0; i < 2; i++) {
        this->fractalTexture[i] = Gfx::CreateResource(rtSetup);
    }

    // create a color render target that holds the fractal state as color texture
    rtSetup.ColorFormat = PixelFormat::RGBA8;
    rtSetup.Sampler.MinFilter = TextureFilterMode::Linear;
    rtSetup.Sampler.MagFilter = TextureFilterMode::Linear;
    this->colorTexture = Gfx::CreateResource(rtSetup);

    // setup draw state for updating the fractal state
    this->fractDrawState.Mesh[0] = fsqMesh;
    auto ps = PipelineSetup::FromLayoutAndShader(fsqLayout, fractalShader);
    ps.RasterizerState.CullFaceEnabled = false;
    ps.DepthStencilState.DepthCmpFunc = CompareFunc::Always;
    ps.BlendState.ColorFormat = PixelFormat::RGBA32F;
    ps.BlendState.DepthFormat = PixelFormat::None;
    this->fractDrawState.Pipeline = Gfx::CreateResource(ps);

    // setup draw state to map fractal state into color texture
    this->colorDrawState.Mesh[0] = fsqMesh;
    Id colorShader = Gfx::CreateResource(ColorShader::Setup());
    ps.Shader = colorShader;
    ps.BlendState.ColorFormat = PixelFormat::RGBA8;
    this->colorDrawState.Pipeline = Gfx::CreateResource(ps);

    Gfx::PopResourceLabel();
}

//------------------------------------------------------------------------------
void
fractal::discard() {
    o_assert_dbg(this->isValid());
    Gfx::DestroyResources(this->label);
    this->label.Invalidate();
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
    const int writeIndex = this->frameIndex & 1;
    const int readIndex  = (this->frameIndex + 1) & 1;
    const Id& readTex = this->fractalTexture[readIndex];
    const Id& writeTex = this->fractalTexture[writeIndex];

    // render next fractal iteration
    this->fractDrawState.FSTexture[Textures::Texture] = readTex;
    this->fractVSParams.Rect = this->rect;
    this->fractFSParams.JuliaPos = this->pos;
    Gfx::ApplyRenderTarget(writeTex, ClearState::ClearNone());
    Gfx::ApplyDrawState(this->fractDrawState);
    Gfx::ApplyUniformBlock(this->fractVSParams);
    Gfx::ApplyUniformBlock(this->fractFSParams);
    Gfx::Draw(0);

    // map current fractal state to color texture
    this->colorDrawState.FSTexture[Textures::Texture] = writeTex;
    Gfx::ApplyRenderTarget(this->colorTexture, ClearState::ClearNone());
    Gfx::ApplyDrawState(this->colorDrawState);
    Gfx::Draw(0);

    if (this->frameIndex >= this->cycleCount) {
        this->frameIndex = 0;
    }
}

} // namespace Julia
