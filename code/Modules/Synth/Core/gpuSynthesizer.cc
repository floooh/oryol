//------------------------------------------------------------------------------
//  gpuSynthesizer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "gpuSynthesizer.h"
#include "Core/Assertion.h"
#include "SynthShaders.h"
#include "Gfx/Gfx.h"
#include "Core/Log.h"

namespace Oryol {
namespace _priv {
    
//------------------------------------------------------------------------------
gpuSynthesizer::gpuSynthesizer() :
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
gpuSynthesizer::~gpuSynthesizer() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
gpuSynthesizer::Setup(const SynthSetup& setupAttrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    
    this->resourceLabel = Gfx::PushResourceLabel();
    const int32 rtWidth  = (synth::BufferNumSamples / 2) / 8;
    const int32 rtHeight = 8;
    auto rtSetup = TextureSetup::RenderTarget(rtWidth, rtHeight);
    this->renderTarget = Gfx::CreateResource(rtSetup);
    
    Id fsqMesh = Gfx::CreateResource(MeshSetup::FullScreenQuad());
    Id shd = Gfx::CreateResource(Shaders::Synth::Setup());
    this->drawState = Gfx::CreateResource(DrawStateSetup::FromMeshAndShader(fsqMesh, shd));
    Gfx::PopResourceLabel();
}

//------------------------------------------------------------------------------
void
gpuSynthesizer::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    Gfx::DestroyResources(this->resourceLabel);
}

//------------------------------------------------------------------------------
bool
gpuSynthesizer::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
void
gpuSynthesizer::Synthesize(const opBundle& bundle) const {

    Gfx::ApplyRenderTarget(this->renderTarget);
    Gfx::ApplyDrawState(this->drawState);
    Gfx::Draw(0);
    Gfx::ReadPixels(bundle.Buffer[0], bundle.BufferNumBytes);
    
    int16* samples = (int16*) bundle.Buffer[0];
    int16 s0 = samples[0];
    int16 s1 = samples[1];
    Log::Info("s0=%d, s1=%d\n", s0, s1);
}

} // namespace _priv
} // namespace Oryol
