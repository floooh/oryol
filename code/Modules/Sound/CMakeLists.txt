fips_begin_module(Sound)
    fips_files(Sound.cc Sound.h)
    fips_dir(Core)
    fips_files(
        SoundSetup.h
        SoundEffectSetup.h
        soundResourceContainer.cc soundResourceContainer.h
        soundEffectBase.cc soundEffectBase.h
        soundEffect.h
        soundMgrBase.cc soundMgrBase.h
        soundMgr.h
        soundEffectFactoryBase.cc soundEffectFactoryBase.h
        soundEffectFactory.h
    )
    if (ORYOL_OPENAL)
        fips_dir(al)
        fips_files(
            sound_al.h
            alSoundEffect.cc alSoundEffect.h
            alSoundMgr.cc alSoundMgr.h
            alSoundEffectFactory.cc alSoundEffectFactory.h
        )
    endif()
    fips_deps(Core IO Resource)
fips_end_module()

