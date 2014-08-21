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
    /// less-operation for sorting by absStartTime
    bool operator<(const item& rhs) const {
        return this->absStartTick < rhs.absStartTick;
    };
    
    Sound sound;
    float32 timeOffset = 0.0f;
    int32 absStartTick = 0;
};
    
} // namespace Synth
} // namespace Oryol