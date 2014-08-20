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
    void EnqueueItem(int32 curTick, const item& item);
    /// generate a buffer worth of data
    void Generate(int32 startTick, void* buffer, int32 bufNumBytes);

    /// set voice volume (0..1)
    void SetVolume(float32 v);
    /// get global volume
    float32 Volume() const;
    
private:
    /// generate samples for a single item
    void generateItemSamples(int32 startTick, int32 endTick, const item& item);

    bool isValid;
    float32 volume;
    Core::Queue<item> itemQueue;
};

//------------------------------------------------------------------------------
inline bool
generator::IsValid() const {
    return this->isValid;
}

} // namespace Synth
} // namespace Oryol
