//------------------------------------------------------------------------------
//  alBufferStreamer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "alBufferStreamer.h"

namespace Oryol {
namespace Synth {
    
//------------------------------------------------------------------------------
alBufferStreamer::alBufferStreamer() :
isValid(false),
source(0) {
    this->allBuffers.Reserve(MaxNumBuffers);
    this->queuedBuffers.Reserve(MaxNumBuffers);
    this->freeBuffers.Reserve(MaxNumBuffers);
}

//------------------------------------------------------------------------------
alBufferStreamer::~alBufferStreamer() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
alBufferStreamer::Setup(const SynthSetup& setupAttrs) {
    o_assert_dbg(!this->isValid);
    
    this->isValid = true;
    
    // FIXME: actually use attrs from SynthSetup!
    
    // generate buffers, initially fill buffer with 0
    int16 silence[synth::BufferNumSamples] = { 0 };
    o_assert_dbg(sizeof(silence) == synth::BufferSize);
    for (int i = 0; i < MaxNumBuffers; i++) {
        ALuint buf = 0;
        alGenBuffers(1, &buf);
        ORYOL_AL_CHECK_ERROR();
        alBufferData(buf, AL_FORMAT_MONO16, silence, sizeof(silence), synth::SampleRate);
        ORYOL_AL_CHECK_ERROR();
        this->allBuffers.Add(buf);
        this->freeBuffers.Enqueue(buf);
    }

    // generate a single playback source
    alGenSources(1, &this->source);
    ORYOL_AL_CHECK_ERROR();
}

//------------------------------------------------------------------------------
void
alBufferStreamer::Discard() {
    o_assert_dbg(this->isValid);
    o_assert_dbg(0 != this->source);
    
    this->isValid = false;
    
    alDeleteSources(1, &this->source);
    ORYOL_AL_CHECK_ERROR();
    this->source = 0;
    
    for (ALuint buf : this->allBuffers) {
        alDeleteBuffers(1, &buf);
        ORYOL_AL_CHECK_ERROR();
    }
    this->allBuffers.Clear();
    this->queuedBuffers.Clear();
    this->freeBuffers.Clear();
}

//------------------------------------------------------------------------------
bool
alBufferStreamer::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
bool
alBufferStreamer::Update() {
    o_assert_dbg(0 != this->source);
    
    // get number of queued and processed buffers
    ALint buffersQueued = 0;
    ALint buffersProcessed = 0;
    alGetSourcei(this->source, AL_BUFFERS_QUEUED, &buffersQueued);
    ORYOL_AL_CHECK_ERROR();
    alGetSourcei(this->source, AL_BUFFERS_PROCESSED, &buffersProcessed);
    ORYOL_AL_CHECK_ERROR();
    
    // recycle buffers that have been processed
    for (int32 i = 0; i < buffersProcessed; i++) {
        ALuint buf = this->queuedBuffers.Dequeue();
        alSourceUnqueueBuffers(this->source, 1, &buf);
        ORYOL_AL_CHECK_ERROR();
        this->freeBuffers.Enqueue(buf);
    }
    
    // check if new data is needed (processed + 1 playing + 1 waiting)
    // FIXME: or: processed + 1 playing?)
    if ((buffersProcessed + 2) >= buffersQueued) {
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
void
alBufferStreamer::Enqueue(const void* ptr, int32 numBytes) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(synth::BufferSize == numBytes);
    
    ALuint buf = this->freeBuffers.Dequeue();
    this->queuedBuffers.Enqueue(buf);

    alBufferData(buf, AL_FORMAT_MONO16, ptr, numBytes, synth::SampleRate);
    ORYOL_AL_CHECK_ERROR();
    alSourceQueueBuffers(this->source, 1, &buf);
    ORYOL_AL_CHECK_ERROR();
    
    // start playback if source is not currently playing (either it never was, or it
    // has stopped because it was starved)
    ALint srcState = 0;
    alGetSourcei(this->source, AL_SOURCE_STATE, &srcState);
    ORYOL_AL_CHECK_ERROR();
    if (AL_PLAYING != srcState) {
        alSourcePlay(this->source);
        ORYOL_AL_CHECK_ERROR();
        Log::Dbg("alBufferStreamer: starting playback\n");
    }
}

    
} // namespace Synth
} // namespace Oryol
