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
    /// @param dt   time from one sample to next in seconds
    /// @param ptr  sample pointer
    /// @param num  number of samples
    typedef std::function<void(float32 dt, int16* sampleBuffer, int32 numSamples)> SampleFuncT;

    /// create with number of samples and SampleFunc
    static SoundEffectSetup FromSampleFunc(float32 duration, int32 bufferFreq, SampleFuncT sampleFunc);

    /// resource locator
    class Locator Locator = Locator::NonShared();
    /// frequency of samples in effect buffer (number of samples per seconds)
    int32 BufferFrequency = 22050;
    /// duration in seconds
    float32 Duration;
    /// optional callback function to setup sample buffer
    SampleFuncT SampleFunc;
};

} // namespace Oryol