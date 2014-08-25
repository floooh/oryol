//------------------------------------------------------------------------------
//  alSoundMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "alSoundMgr.h"
#include "Core/Log.h"
#include "Core/Assert.h"
#include "Core/String/StringBuilder.h"

namespace Oryol {
namespace Synth {

using namespace Core;
    
//------------------------------------------------------------------------------
alSoundMgr::alSoundMgr() :
alcDevice(nullptr),
alcContext(nullptr) {
    // empty
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
    
    // setup an OpenAL context and make it current
    this->alcDevice = alcOpenDevice(NULL);
    if (NULL == this->alcDevice) {
        Log::Warn("alcOpenDevice() failed!\n");
        return;
    }
    this->alcContext = alcCreateContext(this->alcDevice, NULL);
    if (NULL == this->alcContext) {
        Log::Warn("alcCreateContext() failed!\n");
        return;
    }
    if (!alcMakeContextCurrent(this->alcContext)) {
        Log::Warn("alcMakeContextCurrent() failed!\n");
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
alSoundMgr::Update() {
    soundMgrBase::Update();
    if (this->streamer.Update()) {
    
        // need to 'render' new buffers
        static int16 samples[synth::NumVoices][synth::BufferNumSamples];
        const int32 startTick = this->curTick;
        const int32 endTick   = startTick + synth::BufferNumSamples;
        opBundle bundle;
        for (int voiceIndex = 0; voiceIndex < synth::NumVoices; voiceIndex++) {
            bundle.StartTick[voiceIndex] = startTick;
            bundle.EndTick[voiceIndex] = endTick;
            bundle.Buffer[voiceIndex] = samples[voiceIndex];
            bundle.BufferNumBytes = synth::BufferSize;
            this->voices[voiceIndex].GatherOps(startTick, endTick, bundle);
        }
        
        // FIXME: select between cpuSynth and gpuSynth here!
        this->cpuSynth.Synthesize(bundle);
        this->curTick += synth::BufferNumSamples;
        
        // FIXME: only one voice handled at the moment!
        this->streamer.Enqueue(samples[0], sizeof(samples[0]));
    }
}

} // namespace Synth
} // namespace Oryol