//------------------------------------------------------------------------------
//  alBufferStreamer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "alBufferStreamer.h"

namespace Oryol {
namespace _priv {
    
//------------------------------------------------------------------------------
alBufferStreamer::alBufferStreamer() :
isValid(false) {
    for (int i = 0; i < synth::NumVoices; i++) {
        this->voices[i].source = 0;
        this->voices[i].queuedBuffers.Reserve(MaxNumBuffers);
    }
    this->allBuffers.Reserve(MaxNumBuffers);
    this->freeBuffers.Reserve(MaxNumBuffers);
}

//------------------------------------------------------------------------------
alBufferStreamer::~alBufferStreamer() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
alBufferStreamer::Setup(const SynthSetup& synthSetup) {
    o_assert_dbg(!this->isValid);
    
    this->isValid = true;
    this->setup = synthSetup;
    
    // generate buffers, initially fill buffer with 0
    int16 silence[this->setup.NumBufferSamples];
    Memory::Clear(silence, int(sizeof(silence)));
    for (int i = 0; i < MaxNumBuffers; i++) {
        ALuint buf = 0;
        alGenBuffers(1, &buf);
        ORYOL_AL_CHECK_ERROR();
        alBufferData(buf, AL_FORMAT_MONO16, silence, int(sizeof(silence)), this->setup.SampleRate);
        ORYOL_AL_CHECK_ERROR();
        this->allBuffers.Add(buf);
        this->freeBuffers.Enqueue(buf);
    }

    // generate one playback source per voice
    for (int i = 0; i < synth::NumVoices; i++) {
        alGenSources(1, &this->voices[i].source);
        ORYOL_AL_CHECK_ERROR();
        alSourcef(this->voices[i].source, AL_GAIN, this->setup.InitialVolume);
    }
}

//------------------------------------------------------------------------------
void
alBufferStreamer::Discard() {
    o_assert_dbg(this->isValid);

    this->isValid = false;

    for (int i = 0; i < synth::NumVoices; i++) {
        o_assert(this->voices[i].source);
        alDeleteSources(1, &this->voices[i].source);
        ORYOL_AL_CHECK_ERROR();
        this->voices[i].source = 0;
        this->voices[i].queuedBuffers.Clear();
    }
    for (ALuint buf : this->allBuffers) {
        alDeleteBuffers(1, &buf);
        ORYOL_AL_CHECK_ERROR();
    }
    this->allBuffers.Clear();
    this->freeBuffers.Clear();
}

//------------------------------------------------------------------------------
bool
alBufferStreamer::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
void
alBufferStreamer::UpdateVolume(float32 v) {
    for (int voice = 0; voice < synth::NumVoices; voice++) {
        o_assert_dbg(0 != this->voices[voice].source);
        alSourcef(this->voices[voice].source, AL_GAIN, v);
    }
}

//------------------------------------------------------------------------------
bool
alBufferStreamer::Update() {
    bool needsNewData = false;
    for (int voice = 0; voice < synth::NumVoices; voice++) {
        o_assert_dbg(0 != this->voices[voice].source);
        
        // get number of queued and processed buffers
        ALint buffersQueued = 0;
        ALint buffersProcessed = 0;
        alGetSourcei(this->voices[voice].source, AL_BUFFERS_QUEUED, &buffersQueued);
        ORYOL_AL_CHECK_ERROR();
        alGetSourcei(this->voices[voice].source, AL_BUFFERS_PROCESSED, &buffersProcessed);
        ORYOL_AL_CHECK_ERROR();
        
        // recycle buffers that have been processed
        for (int32 i = 0; i < buffersProcessed; i++) {
            ALuint buf = this->voices[voice].queuedBuffers.Dequeue();
            alSourceUnqueueBuffers(this->voices[voice].source, 1, &buf);
            ORYOL_AL_CHECK_ERROR();
            this->freeBuffers.Enqueue(buf);
        }
        
        // check if new data is needed (processed + 1 playing + 1 waiting)
        // FIXME: or: processed + 1 playing?)
        needsNewData |= (buffersProcessed + 2) >= buffersQueued;
    }
    return needsNewData;
}

//------------------------------------------------------------------------------
void
alBufferStreamer::Enqueue(int32 voice, const void* ptr, int32 numBytes) {
    o_assert_dbg(this->isValid);
    o_assert_dbg((this->setup.NumBufferSamples * int(sizeof(int16))) == numBytes);
    
    ALuint buf = this->freeBuffers.Dequeue();
    this->voices[voice].queuedBuffers.Enqueue(buf);

    alBufferData(buf, AL_FORMAT_MONO16, ptr, numBytes, this->setup.SampleRate);
    ORYOL_AL_CHECK_ERROR();
    alSourceQueueBuffers(this->voices[voice].source, 1, &buf);
    ORYOL_AL_CHECK_ERROR();
    
    // start playback if source is not currently playing (either it never was, or it
    // has stopped because it was starved)
    ALint srcState = 0;
    alGetSourcei(this->voices[voice].source, AL_SOURCE_STATE, &srcState);
    ORYOL_AL_CHECK_ERROR();
    if (AL_PLAYING != srcState) {
        alSourcePlay(this->voices[voice].source);
        ORYOL_AL_CHECK_ERROR();
        Log::Dbg("alBufferStreamer: starting playback\n");
    }
}

} // namespace _priv
} // namespace Oryol
