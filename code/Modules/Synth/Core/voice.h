#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Synth::generator
    @brief wave form generator
    
    Converts sound items into actual in chunk-sized blocks of waveform data.
*/
#include "Core/Types.h"
#include "Synth/Core/SynthSetup.h"
#include "Synth/Core/item.h"
#include "Synth/Core/synth.h"
#include "Core/Containers/Queue.h"

namespace Oryol {
namespace Synth {
    
class generator {
public:
    /// constructor
    generator();
    /// destructor
    ~generator();
    
    /// setup the synthesizer object
    void Setup(const SynthSetup& setupAttrs);
    /// discard the synthesizer object
    void Discard();
    /// return true if object has been setup
    bool IsValid() const;
    
    /// enqueue a sound item
    void PushItem(const item& item);
    /// generate a buffer worth of data
    void Generate(int32 startTick, void* buffer, int32 bufNumBytes);

private:
    /// generate sample through the waveform oscillator
    float32 oscillator();

    bool isValid;
    int32 gateTick;         // the tick when the gate bit was switch on or off
    Sound curSound;         // the current sound state
    float32 oscillatorPos;  // 'position' of the oscillator
    float32 oscillatorStep; // current step rate of the oscillator
    Core::Queue<item> itemQueue;
};

//------------------------------------------------------------------------------
inline bool
generator::IsValid() const {
    return this->isValid;
}

} // namespace Synth
} // namespace Oryol
