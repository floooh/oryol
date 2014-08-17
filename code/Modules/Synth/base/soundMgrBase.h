#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Synth::soundMgrBase
    @brief sound manager base class
*/
#include "Synth/Core/SynthSetup.h"
#include "Synth/Core/item.h"
#include "Time/TimePoint.h"
#include "Time/Duration.h"
#include "Core/Containers/Queue.h"

namespace Oryol {
namespace Synth {
    
class soundMgrBase {
public:
    /// constructor
    soundMgrBase();
    /// destructor
    ~soundMgrBase();
    
    /// setup the sound system
    void Setup(const SynthSetup& setupAttrs);
    /// discard the sound system
    void Discard();
    /// return true if has been setup
    bool IsValid() const;
    
    /// update the sound system
    void Update(Time::Duration timeDiff);
    
    /// push an item onto the sound queue
    void PushItem(item item);
    
protected:
    bool isValid;
    SynthSetup setup;
    float64 curTime;
    Core::Queue<item> soundQueue;
};
    
} // namespace Synth
} // namespace Oryol