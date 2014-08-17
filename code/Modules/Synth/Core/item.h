#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Synth::item
    @brief an enqueued sound 'work item'
*/
#include "Synth/Core/Sound.h"
#include "Time/TimePoint.h"

namespace Oryol {
namespace Synth {
    
struct item {
    /// construct with params
    item(const Sound& snd, float32 frq, float32 vol, float32 offs) :
        sound(snd),
        frequency(frq),
        volume(vol),
        timeOffset(offs),
        absStartTime(0.0),
        absEndTime(0.0) { }

    /// less-operation for sorting by absStartTime
    bool operator<(const item& rhs) const {
        return this->absStartTime < rhs.absStartTime;
    };
    
    Sound sound;
    float32 frequency = 1.0f;
    float32 volume = 1.0f;
    float32 timeOffset = 0.0f;
    float64 absStartTime;
    float64 absEndTime;
};
    
} // namespace Synth
} // namespace Oryol