#-------------------------------------------------------------------------------
#   oryol Synth module (chip-tune sound)
#-------------------------------------------------------------------------------

fips_begin_module(Synth)
    fips_files(Synth.cc Synth.h)
    fips_dir(Core)
    fips_files(
        SynthOp.h
        cpuSynthesizer.cc cpuSynthesizer.h
        gpuSynthesizer.cc gpuSynthesizer.h
        opBundle.h
        soundMgr.h
        synth.h
        voice.cc voice.h
        voiceTrack.cc voiceTrack.h
    )
    fips_dir(base)
    fips_files(soundMgrBase.cc soundMgrBase.h)
    fips_dir(shaders)
    fips_generate(TYPE Shader FROM SynthShaders.shd)
    if (FIPS_OSX OR FIPS_EMSCRIPTEN)
        fips_dir(al)
        fips_files(
            al.h
            alBufferStreamer.cc alBufferStreamer.h
            alSoundMgr.cc alSoundMgr.h
        )
    endif() 
    fips_deps(Resource Core)
fips_end_module()

