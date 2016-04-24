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
    typedef std::function<void(float dt, int16_t* sampleBuffer, int numSamples)> SampleFuncT;

    /// create with number of samples and SampleFunc
    static SoundEffectSetup FromSampleFunc(int numVoices, float duration, int bufferFreq, SampleFuncT sampleFunc);

    /// resource locator
    class Locator Locator = Locator::NonShared();
    /// frequency of samples in effect buffer (number of samples per seconds)
    int BufferFrequency = 22050;
    /// duration in seconds
    float Duration;
    /// optional callback function to setup sample buffer
    SampleFuncT SampleFunc;
    /// max number of parallel voices for this sound effect
    static const int MaxNumVoices = 16;
    /// number of parallel voices this sound can play
    int NumVoices = MaxNumVoices;
};

} // namespace Oryol