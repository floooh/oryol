#-------------------------------------------------------------------------------
#   oryol Input module
#-------------------------------------------------------------------------------
fips_begin_module(Input)
    fips_vs_warning_level(3)
    fips_files(
        Input.cc Input.h
        InputTypes.h InputTypes.cc
    )
    fips_dir(private)
    fips_files(
        gestureDetectors.h gestureDetectors.cc
        inputDispatcher.cc inputDispatcher.h
        inputDevices.h inputDevices.cc
        inputMgrBase.h inputMgrBase.cc
        inputMgr.h
    )
    if (FIPS_ANDROID)
        fips_dir(private/android)
        fips_files(androidInputMgr.cc androidInputMgr.h)
    endif()
    if (FIPS_EMSCRIPTEN)
        fips_dir(private/emsc)
        fips_files(emscInputMgr.cc emscInputMgr.h)
    endif()
    if (FIPS_IOS)
        fips_dir(private/ios)
        fips_files(iosInputMgr.mm iosInputMgr.h)
        fips_frameworks_osx(CoreMotion)
    endif()
    if (ORYOL_OPENGL)
        if (FIPS_RASPBERRYPI)
            fips_dir(private/raspi)
            fips_files(raspiInputMgr.cc raspiInputMgr.h)
        elseif (FIPS_MACOS OR FIPS_WINDOWS OR FIPS_LINUX)
            fips_dir(private/glfw)
            fips_files(glfwInputMgr.cc glfwInputMgr.h)
        endif()
    endif()
    if (FIPS_MACOS AND ORYOL_METAL)
        fips_dir(private/osx)
        fips_files(osxInputMgr.cc osxInputMgr.h)
    endif()
    if (ORYOL_D3D11)
        fips_dir(private/win)
        fips_files(winInputMgr.cc winInputMgr.h)
    endif()
    fips_deps(Core Gfx)
fips_end_module()

