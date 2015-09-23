//------------------------------------------------------------------------------
//  tbOryolBitmap.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "tbOryolBitmap.h"
#include "tbOryolBatchRenderer.h"
#include "tb_bitmap_fragment.h"
#include "TBUIShaders.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
tbOryolBitmap::tbOryolBitmap(tbOryolBatchRenderer* rnd) :
renderer(rnd),
width(0),
height(0),
label(ResourceLabel::Invalid) {
    // empty
}

//------------------------------------------------------------------------------
tbOryolBitmap::~tbOryolBitmap() {
    if (this->textureBlock.IsValid()) {
        this->destroyTexture();
    }
}

//------------------------------------------------------------------------------
bool
tbOryolBitmap::Init(int w, int h, tb::uint32* data) {
    o_assert_dbg(tb::TBGetNearestPowerOfTwo(w) == w);
    o_assert_dbg(tb::TBGetNearestPowerOfTwo(h) == h);
    this->width = w;
    this->height = h;
    this->createTexture(data);
    return true;
}

//------------------------------------------------------------------------------
void
tbOryolBitmap::SetData(uint32* data) {
    o_assert_dbg(this->textureBlock.IsValid());
    this->destroyTexture();
    this->createTexture(data);
}

//------------------------------------------------------------------------------
void
tbOryolBitmap::createTexture(tb::uint32* data) {
    o_assert_dbg(!this->textureBlock.IsValid());
    o_assert_dbg(this->renderer && this->renderer->shader.IsValid());

    const int byteSize = this->width * this->height * sizeof(tb::uint32);

    this->label = Gfx::PushResourceLabel();
    auto texSetup = TextureSetup::FromPixelData(this->width, this->height, 1, TextureType::Texture2D, PixelFormat::RGBA8);
    texSetup.Sampler.WrapU = TextureWrapMode::Repeat;
    texSetup.Sampler.WrapV = TextureWrapMode::Repeat;
    texSetup.Sampler.MinFilter = TextureFilterMode::Nearest;
    texSetup.Sampler.MagFilter = TextureFilterMode::Nearest;
    texSetup.ImageSizes[0][0] = byteSize;
    Id texture = Gfx::CreateResource(texSetup, data, byteSize);

    auto tbSetup = Shaders::TBUIShader::FSTextures::Setup(this->renderer->shader);
    tbSetup.Slot[Shaders::TBUIShader::FSTextures::Texture] = texture;
    this->textureBlock = Gfx::CreateResource(tbSetup);
    
    Gfx::PopResourceLabel();
}

//------------------------------------------------------------------------------
void
tbOryolBitmap::destroyTexture() {
    o_assert_dbg(this->textureBlock.IsValid());
    this->renderer->deferDeleteTexture(this->label);
    this->textureBlock.Invalidate();
    this->label = ResourceLabel::Invalid;
}

} // namespace _priv
} // namespace Oryol