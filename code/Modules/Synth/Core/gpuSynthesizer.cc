//------------------------------------------------------------------------------
//  gpuSynthesizer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Assert.h"
#include "gpuSynthesizer.h"

namespace Oryol {
namespace Synth {
    
//------------------------------------------------------------------------------
gpuSynthesizer::gpuSynthesizer() :
isValid(false) {
    // FIXME!
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

    // FIMXE: setup draw state
}

//------------------------------------------------------------------------------
void
gpuSynthesizer::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    
    // FIXME: discard draw state
}

//------------------------------------------------------------------------------
bool
gpuSynthesizer::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
void
gpuSynthesizer::Synthesize(const opBundle& bundle) const {

    // FIXME: RENDER THE AUDIO SAMPLES!
    
}

} // namespace Synth
} // namespace Oryol