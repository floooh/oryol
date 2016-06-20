#-------------------------------------------------------------------------------
#   oryol Input module
#-------------------------------------------------------------------------------
fips_begin_module(Input)
    fips_vs_warning_level(3)
    fips_files(Input.cc Input.h)
    fips_dir(Core)
    fips_files(
        InputSetup.h InputEnums.h InputEvent.h
        Key.cc Key.h
        inputDispatcher.cc inputDispatcher.h
        gamepadDevice.cc gamepadDevice.h
        keyboardDevice.cc keyboardDevice.h
        mouseDevice.cc mouseDevice.h
        sensorDevice.h
        touchpadDevice.cc touchpadDevice.h
        inputMgrBase.h inputMgrBase.cc
        inputMgr.h
    )
    fips_dir(touch)
    fips_files(
        gestureState.h
        panDetector.cc panDetector.h
        pinchDetector.cc pinchDetector.h
        tapDetector.cc tapDetector.h
        touchEvent.cc touchEvent.h
    )
    if (FIPS_ANDROID)
        fips_dir(android)
        fips_files(androidInputMgr.cc androidInputMgr.h)
    endif()
    if (FIPS_EMSCRIPTEN)
        fips_dir(emsc)
        fips_files(emscInputMgr.cc emscInputMgr.h)
    endif()
    if (FIPS_IOS)
        fips_dir(ios)
        fips_files(iosInputMgr.mm iosInputMgr.h)
        fips_frameworks_osx(CoreMotion)
    endif()
    if (FIPS_PNACL)
        fips_dir(pnacl)
        fips_files(pnaclInputMgr.cc pnaclInputMgr.h)
    endif()
    if (ORYOL_OPENGL OR ORYOL_VULKAN)
        if (FIPS_RASPBERRYPI)
            fips_dir(raspi)
            fips_files(raspiInputMgr.cc raspiInputMgr.h)
        elseif (FIPS_MACOS OR FIPS_WINDOWS OR FIPS_LINUX)
            fips_dir(glfw)
            fips_files(glfwInputMgr.cc glfwInputMgr.h)
        endif()
    endif()
    if (FIPS_MACOS AND ORYOL_METAL)
        fips_dir(osx)
        fips_files(osxInputMgr.cc osxInputMgr.h)
    endif()
    if (ORYOL_D3D11 OR ORYOL_D3D12)
        fips_dir(win)
        fips_files(winInputMgr.cc winInputMgr.h)
    endif()
    fips_deps(Core Gfx)
fips_end_module()

