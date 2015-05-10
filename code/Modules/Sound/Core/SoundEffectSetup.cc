//------------------------------------------------------------------------------
//  SoundEffectSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "SoundEffectSetup.h"

namespace Oryol {

//------------------------------------------------------------------------------
SoundEffectSetup
SoundEffectSetup::FromSampleFunc(float32 dur, int32 freq, SampleFuncT sampleFunc) {
    o_assert_dbg(dur > 0.0f);
    o_assert_dbg(freq > 0);
    SoundEffectSetup setup;
    setup.Duration = dur;
    setup.BufferFrequency = freq;
    setup.SampleFunc = sampleFunc;
    return setup;
}

} // namespace Oryol