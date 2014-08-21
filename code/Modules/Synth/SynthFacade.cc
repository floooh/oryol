//------------------------------------------------------------------------------
//  SynthFacade.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "SynthFacade.h"
#include "Synth/Core/item.h"

namespace Oryol {
namespace Synth {

OryolLocalSingletonImpl(SynthFacade);

//------------------------------------------------------------------------------
SynthFacade::SynthFacade(const SynthSetup& setupAttrs) {
    this->soundManager.Setup(setupAttrs);
}

//------------------------------------------------------------------------------
SynthFacade::~SynthFacade() {
    this->soundManager.Discard();
}

//------------------------------------------------------------------------------
void
SynthFacade::Update() {
    this->soundManager.Update();
}

//------------------------------------------------------------------------------
void
SynthFacade::Play(uint32 voice, const Sound& sound, float32 timeOffset) {
    this->soundManager.Play(voice, sound, timeOffset);
}

} // namespace Synth
} // namespace Oryol
