//------------------------------------------------------------------------------
//  alSoundMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "alSoundMgr.h"
#include "Core/Log.h"
#include "Core/Assertion.h"
#include "Core/String/StringBuilder.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
alSoundMgr::alSoundMgr() :
alcDevice(nullptr),
alcContext(nullptr),
sampleBufferSize(0) {
    this->sampleBuffers.Fill(nullptr);
}

//------------------------------------------------------------------------------
alSoundMgr::~alSoundMgr() {
    o_assert_dbg(nullptr == this->alcDevice);
    o_assert_dbg(nullptr == this->alcContext);
}

//------------------------------------------------------------------------------
void
alSoundMgr::Setup(const SynthSetup& setupAttrs) {
    o_assert_dbg(!this->isValid);
    o_assert_dbg(nullptr == this->alcDevice);
    o_assert_dbg(nullptr == this->alcContext);

    soundMgrBase::Setup(setupAttrs);

    // allocate sample buffers
    this->sampleBufferSize = this->setup.NumBufferSamples*sizeof(int16);
    for (int i=0; i<synth::NumVoices; i++) {
        this->sampleBuffers[i] = (int16*) Memory::Alloc(this->sampleBufferSize);
    }
    
    // setup an OpenAL context and make it current
    this->alcDevice = alcOpenDevice(NULL);
    if (NULL == this->alcDevice) {
        o_warn("alcOpenDevice() failed!\n");
        return;
    }
    this->alcContext = alcCreateContext(this->alcDevice, NULL);
    if (NULL == this->alcContext) {
        o_warn("alcCreateContext() failed!\n");
        return;
    }
    if (!alcMakeContextCurrent(this->alcContext)) {
        o_warn("alcMakeContextCurrent() failed!\n");
        return;
    }
    this->PrintALInfo();
    
    // setup the buffer streamer
    this->streamer.Setup(setupAttrs);
}

//------------------------------------------------------------------------------
void
alSoundMgr::Discard() {
    o_assert_dbg(this->isValid);
    
    this->streamer.Discard();
    if (nullptr != this->alcContext) {
        alcDestroyContext(this->alcContext);
        this->alcContext = nullptr;
    }
    if (nullptr != this->alcDevice) {
        alcCloseDevice(this->alcDevice);
        this->alcDevice = nullptr;
    }
    for (int i=0; i<synth::NumVoices; i++) {
        if (this->sampleBuffers[i]) {
            Memory::Free(this->sampleBuffers[i]);
            this->sampleBuffers[i] = nullptr;
        }
    }
    
    soundMgrBase::Discard();
}

//------------------------------------------------------------------------------
void
alSoundMgr::PrintALInfo() {
    const ALCchar* alcStr = alcGetString(this->alcDevice, ALC_DEVICE_SPECIFIER);
    if (alcStr) {
        Log::Info("ALC_DEVICE_SPECIFIER: %s\n", alcStr);
    }
    alcStr = alcGetString(this->alcDevice, ALC_EXTENSIONS);
    if (alcStr) {
        StringBuilder strBuilder(alcStr);
        strBuilder.SubstituteAll(" ", "\n");
        Log::Info("ALC_EXTENSIONS:\n%s\n", strBuilder.AsCStr());
    }
    const ALchar* alStr = alGetString(AL_VERSION);
    if (alStr) {
        Log::Info("AL_VERSION: %s\n", alStr);
    }
    alStr = alGetString(AL_RENDERER);
    if (alStr) {
        Log::Info("AL_RENDERER: %s\n", alStr);
    }
    alStr = alGetString(AL_VENDOR);
    if (alStr) {
        Log::Info("AL_VENDOR: %s\n", alStr);
    }
    alStr = alGetString(AL_EXTENSIONS);
    if (alStr) {
        StringBuilder strBuilder(alStr);
        strBuilder.SubstituteAll(" ", "\n");
        Log::Info("AL_EXTENSIONS:\n%s\n", strBuilder.AsCStr());
    }
}

//------------------------------------------------------------------------------
void
alSoundMgr::UpdateVolume(float32 vol) {
    this->streamer.UpdateVolume(vol);
}

//------------------------------------------------------------------------------
void
alSoundMgr::Update() {
    soundMgrBase::Update();
    if (this->streamer.Update()) {
    
        // need to 'render' new buffers
        const int32 startTick = this->curTick;
        const int32 endTick = startTick + this->setup.NumBufferSamples;
        opBundle bundle;
        for (int voice = 0; voice < synth::NumVoices; voice++) {
            bundle.StartTick[voice] = startTick;
            bundle.EndTick[voice] = endTick;
            bundle.Buffer[voice] = this->sampleBuffers[voice];
            bundle.BufferNumBytes = this->sampleBufferSize;
            this->voices[voice].GatherOps(startTick, endTick, bundle);
        }
        this->cpuSynth.Synthesize(bundle);
        this->curTick += this->setup.NumBufferSamples;
        for (int voice = 0; voice < synth::NumVoices; voice++) {
            this->streamer.Enqueue(voice, this->sampleBuffers[voice], this->sampleBufferSize);
        }
    }
}

} // namespace _priv
} // namespace Oryol
