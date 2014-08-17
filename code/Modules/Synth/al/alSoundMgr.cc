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
    
    // output some information about this OpenAL implementation
    const ALCchar* str = alcGetString(this->alcDevice, ALC_DEVICE_SPECIFIER);
    if (str) {
        Log::Info("ALC_DEVICE_SPECIFIER: %s\n", str);
    }
    str = alcGetString(this->alcDevice, ALC_EXTENSIONS);
    if (str) {
        StringBuilder strBuilder(str);
        strBuilder.SubstituteAll(" ", "\n");
        Log::Info("ALC_EXTENSIONS:\n%s\n", strBuilder.AsCStr());
    }
}

//------------------------------------------------------------------------------
void
alSoundMgr::Discard() {
    o_assert_dbg(this->isValid);
    
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

} // namespace Synth
} // namespace Oryol