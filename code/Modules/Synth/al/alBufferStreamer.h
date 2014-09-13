#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Synth::alBufferStreamer.h
    @brief manage buffer queueing and reuse
    
    This is the heart of the buffer streaming system. A number of small buffers
    are preallocated (the size and samplerate defines the minimal latency of 
    the whole system). Free buffers will be pulled from a free-queue,
    filled with sample data, enqueued for playback. When completely played,
    buffers will be put back into the free-queue, forming a circle. 
    
    Latency vs. under-runs: One buffer only holds data for a couple of 
    milliseconds (one or two render-frames). This defines the minimal
    latency of the system. However, if the game fails to render at this
    frame rate, buffer underruns can happen resulting in audio artefacts.
    
    Unfortunately, OpenAL doesn't allow to intercept/remove queued buffers
    which are still pending for playback, so a compromise must be made
    between latency and buffer underruns. The latency has to be the minimal
    frame rate an application is expected to render.
*/
#include "Core/Containers/Array.h"
#include "Core/Containers/Queue.h"
#include "Synth/Core/SynthSetup.h"
#include "Synth/al/al.h"
#include "Synth/Core/synth.h"

namespace Oryol {
namespace Synth {
    
class alBufferStreamer {
public:
    /// constructor
    alBufferStreamer();
    /// destructor
    ~alBufferStreamer();
    
    /// setup the streamer
    void Setup(const SynthSetup& setupAttrs);
    /// discard the streamer
    void Discard();
    /// return true if streamer object has been setup
    bool IsValid() const;
    
    /// update the streamer, returns true if new data is needed
    bool Update();
    /// enqueue new data into the streamer
    void Enqueue(const void* ptr, int32 numBytes);
    
private:
    static const int32 MaxNumBuffers = 8;

    bool isValid;
    ALuint source;
    Array<ALuint> allBuffers;
    Queue<ALuint> queuedBuffers;
    Queue<ALuint> freeBuffers;
};
    
} // namespace Synth
} // namespace Oryol
 
 