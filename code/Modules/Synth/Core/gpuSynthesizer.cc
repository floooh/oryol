//------------------------------------------------------------------------------
//  gpuSynthesizer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "gpuSynthesizer.h"
#include "Core/Assert.h"
#include "Synth/shaders/SynthShaders.h"
#include "Render/RenderFacade.h"
#include "Core/Log.h"

using namespace Oryol::Render;
using namespace Oryol::Resource;

namespace Oryol {
namespace Synth {
    
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
    RenderFacade* render = RenderFacade::Instance();
    
    // setup a rendering resources
    const int32 rtWidth  = (synth::BufferNumSamples / 2) / 8;
    const int32 rtHeight = 8;
    this->renderTarget = render->CreateResource(TextureSetup::AsRenderTarget("_synthRT", rtWidth, rtHeight, PixelFormat::RGBA8));
    Id fsqMesh = render->CreateResource(MeshSetup::CreateFullScreenQuad("_synthFSQMesh"));
    Id prog = render->CreateResource(Shaders::Synth::CreateSetup());
    this->drawState = render->CreateResource(DrawStateSetup("_synthDS", fsqMesh, prog, 0));
    render->ReleaseResource(prog);
    render->ReleaseResource(fsqMesh);
}

//------------------------------------------------------------------------------
void
gpuSynthesizer::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    
    RenderFacade* render = RenderFacade::Instance();
    render->ReleaseResource(this->renderTarget);
    this->renderTarget.Invalidate();
    render->ReleaseResource(this->drawState);
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

    RenderFacade* render = RenderFacade::Instance();
    render->ApplyOffscreenRenderTarget(this->renderTarget);
    render->ApplyDrawState(this->drawState);
    render->Draw(0);
    render->ReadPixels(bundle.Buffer[0], bundle.BufferNumBytes);
    
    int16* samples = (int16*) bundle.Buffer[0];
    int16 s0 = samples[0];
    int16 s1 = samples[1];
    Core::Log::Info("s0=%d, s1=%d\n", s0, s1);
}

} // namespace Synth
} // namespace Oryol