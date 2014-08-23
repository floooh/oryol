//------------------------------------------------------------------------------
//  SynthFacade.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "SynthFacade.h"

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
SynthFacade::AddOp(int32 voice, int32 track, const Op& op, float32 timeOffset) {
    this->soundManager.AddOp(voice, track, op, timeOffset);
}

} // namespace Synth
} // namespace Oryol
