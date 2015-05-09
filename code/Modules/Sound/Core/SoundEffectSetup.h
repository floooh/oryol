#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::SoundEffectSetup
    @ingroup Sound
    @brief setup object for a canned sound effect
*/
#include "Core/Types.h"
#include "Resource/Locator.h"
#include <functional>

namespace Oryol {

class SoundEffectSetup {
public:
    /// optional callback function to setup sound effect samples
    /// @param t    timestamp in seconds
    /// @param dt   delta-time from one sample to next
    /// @param ptr  sample pointer
    /// @param num  number of samples
    typedef std::function<void(float32 t, float32 dt, int16* sampleBuffer, int32 numSamples)> SampleFuncT;

    /// create with number of samples and SampleFunc
    static SoundEffectSetup FromSampleFunc(int32 duration, SampleFuncT sampleFunc);

    /// resource locator
    class Locator Locator;
    /// base frequency of sound effect in Hz
    int32 Frequency = 22050;
    /// duration in millseconds
    int32 Duration;
    /// optional callback function to setup sample buffer
    SampleFuncT SampleFunc;
};

} // namespace Oryol