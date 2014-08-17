//------------------------------------------------------------------------------
//  SynthFacade.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "SynthFacade.h"

namespace Oryol {
namespace Synth {

OryolLocalSingletonImpl(SynthFacade);

//------------------------------------------------------------------------------
SynthFacade::SynthFacade(const SynthSetup& setupAttrs) :
curTick(0) {
    this->soundManager.Setup(setupAttrs);
}

//------------------------------------------------------------------------------
SynthFacade::~SynthFacade() {
    this->soundManager.Discard();
}

//------------------------------------------------------------------------------
void
SynthFacade::Update() {
    this->soundManager.Update(this->curTick);
    this->curTick++;
}

//------------------------------------------------------------------------------
int32
SynthFacade::CurrentTick() const {
    return this->curTick;
}

} // namespace Synth
} // namespace Oryol
