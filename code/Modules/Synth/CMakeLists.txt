#-------------------------------------------------------------------------------
#   oryol Synth module (chip-tune sound)
#-------------------------------------------------------------------------------

fips_begin_module(Synth)
    fips_vs_warning_level(3)
    fips_files(Synth.cc Synth.h)
    oryol_shader(SynthShaders.shd)
    fips_dir(Core)
    fips_files(
        SynthOp.h
        SynthSetup.h
        cpuSynthesizer.cc cpuSynthesizer.h
        opBundle.h
        soundMgr.h
        synth.h
        voice.cc voice.h
        voiceTrack.cc voiceTrack.h
    )
    fips_dir(base)
    fips_files(soundMgrBase.cc soundMgrBase.h)
    if (FIPS_OSX OR FIPS_EMSCRIPTEN)
        fips_dir(al)
        fips_files(
            al.h
            alBufferStreamer.cc alBufferStreamer.h
            alSoundMgr.cc alSoundMgr.h
        )
    endif() 
    fips_deps(Resource Core)
    if (FIPS_OSX)
        fips_frameworks_osx(OpenAL)
    endif()
fips_end_module()

