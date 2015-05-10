//------------------------------------------------------------------------------
//  SoundEffectSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "SoundEffectSetup.h"

namespace Oryol {

//------------------------------------------------------------------------------
SoundEffectSetup
SoundEffectSetup::FromSampleFunc(float32 duration, SampleFuncT sampleFunc) {
    SoundEffectSetup setup;
    setup.Duration = duration;
    setup.SampleFunc = sampleFunc;
    return setup;
}

} // namespace Oryol