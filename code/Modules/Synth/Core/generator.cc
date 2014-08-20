//------------------------------------------------------------------------------
//  generator.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "generator.h"
#include "Core/Assert.h"

namespace Oryol {
namespace Synth {
    
//------------------------------------------------------------------------------
generator::generator() :
isValid(false),
volume(1.0f) {
    this->itemQueue.Reserve(32);
}

//------------------------------------------------------------------------------
generator::~generator() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
generator::Setup(const SynthSetup& setupAttrs) {
    o_assert_dbg(!this->isValid);
    o_assert_dbg(this->itemQueue.Empty());
    
    this->isValid = true;
    this->volume = 1.0f;
}

//------------------------------------------------------------------------------
void
generator::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    this->itemQueue.Clear();
}

//------------------------------------------------------------------------------
void
generator::SetVolume(float v) {
    o_assert((v >= 0.0f) && (v <= 1.0f));
    this->volume = v;
}

//------------------------------------------------------------------------------
float32
generator::Volume() const {
    return this->volume;
}
    
//------------------------------------------------------------------------------
void
generator::EnqueueItem(int32 curTick, const item& item) {
    o_assert_dbg(this->isValid);
    struct item newItem = item;
    newItem.absStartTick = curTick + synth::TimeToTicks(item.timeOffset, synth::SampleRate);
    newItem.absEndTick = newItem.absStartTick + synth::TimeToTicks(item.sound.Duration(), synth::SampleRate);
    this->itemQueue.Insert(item);
}

//------------------------------------------------------------------------------
void
generator::Generate(int32 startTick, void* buffer, int32 bufNumBytes) {
    o_assert_dbg(this->isValid);
    o_assert(synth::BufferSize == bufNumBytes);
    
    // the sample tick range covered by the buffer
    const int32 endTick = startTick + synth::BufferNumSamples;

    // drop any sound items which are before this buffer range
    while (this->itemQueue.Peek(0).absEndTick < startTick) {
        this->itemQueue.Dequeue();
    }
    
    // first fill buffer with silence
    Memory::Clear(buffer, synth::BufferSize);
    
    // for each sound item overlapping this buffer time range...
    int i = 0;
    while (this->itemQueue.Peek(i).absStartTick < endTick) {
        this->generateItemSamples(startTick, endTick, this->itemQueue.Peek(i), (int16*) buffer);
        i++;
    }
}

//------------------------------------------------------------------------------
void
generator::generateItemSamples(int32 bufStartTick, int32 bufEndTick, const item& item, int16* buffer) {
    
    // item start and end tick clamped against buffer range
    int32 itemStartTick = item.absStartTick;
    int32 itemEndTick   = item.absEndTick;
    if (itemStartTick < bufStartTick) {
        itemStartTick = bufStartTick;
    }
    if (itemEndTick > bufEndTick) {
        itemEndTick = bufEndTick;
    }
    int32 itemTickOffset = item.absStartTick - itemStartTick;
    
    float32 t = synth::TicksToSeconds(itemTickOffset);
    const float32 dx = synth::TicksToSeconds(1);
    const int32 startIndex = itemStartTick - bufStartTick;
    const int32 endIndex = itemEndTick - bufStartTick;
    for (int32 bufIndex = startIndex; bufIndex < endIndex; bufIndex++, t+=dx) {
        
        // lookup waveform sample
        float32 waveFormTime = t * synth::WaveFormDuration * item.pitch;
        float32 v;
        switch (item.sound.Wave()) {
            case WaveForm::Sine:        v = synth::Sine(waveFormTime); break;
            case WaveForm::Triangle:    v = synth::Triangle(waveFormTime); break;
            case WaveForm::SawTooth:    v = synth::SawTooth(waveFormTime); break;
            case WaveForm::Square:      v = synth::Square(waveFormTime); break;
            case WaveForm::Noise:       v = synth::Noise(waveFormTime); break;
            default:                    v = 0.0f;
        }
        
        // apply volume
        v = v * item.volume * this->volume;
        
        // FIXME: apply ADSR hull
        
        // FIXME: apply filters
        
        // convert to int16 and write to buffer
        int16 sample = v * ((1<<15)-1);
        buffer[bufIndex] = sample;
    }
}

} // namespace Synth
} // namespace Oryol
