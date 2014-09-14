//------------------------------------------------------------------------------
//  gpuSynthesizer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "gpuSynthesizer.h"
#include "Core/Assert.h"
#include "Synth/shaders/SynthShaders.h"
#include "Render/Render.h"
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
    
    // setup a rendering resources
    const int32 rtWidth  = (synth::BufferNumSamples / 2) / 8;
    const int32 rtHeight = 8;
    auto rtSetup = TextureSetup::RenderTarget(rtWidth, rtHeight);
    this->renderTarget = Render::CreateResource(rtSetup);
    
    Id fsqMesh = Render::CreateResource(MeshSetup::FullScreenQuad());
    Id prog = Render::CreateResource(Shaders::Synth::CreateSetup());
    this->drawState = Render::CreateResource(DrawStateSetup::FromMeshAndProg(fsqMesh, prog));
    Render::ReleaseResource(prog);
    Render::ReleaseResource(fsqMesh);
}

//------------------------------------------------------------------------------
void
gpuSynthesizer::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    
    Render::ReleaseResource(this->renderTarget);
    this->renderTarget.Invalidate();
    Render::ReleaseResource(this->drawState);
    this->drawState.Invalidate();
}

//------------------------------------------------------------------------------
bool
gpuSynthesizer::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
void
gpuSynthesizer::Synthesize(const opBundle& bundle) const {

    Render::ApplyOffscreenRenderTarget(this->renderTarget);
    Render::ApplyDrawState(this->drawState);
    Render::Draw(0);
    Render::ReadPixels(bundle.Buffer[0], bundle.BufferNumBytes);
    
    int16* samples = (int16*) bundle.Buffer[0];
    int16 s0 = samples[0];
    int16 s1 = samples[1];
    Log::Info("s0=%d, s1=%d\n", s0, s1);
}

} // namespace _priv
} // namespace Oryol
