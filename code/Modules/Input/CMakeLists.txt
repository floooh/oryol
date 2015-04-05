#-------------------------------------------------------------------------------
#   oryol Input module
#-------------------------------------------------------------------------------
fips_begin_module(Input)
    fips_files(Input.cc Input.h)
    fips_generate(TYPE MessageProtocol FROM InputProtocol.yml)
    fips_dir(Core)
    fips_files(
        CursorMode.h
        Gamepad.cc Gamepad.h
        InputSetup.h
        Key.cc Key.h
        Keyboard.cc Keyboard.h
        Mouse.cc Mouse.h
        Sensors.h
        Touchpad.cc Touchpad.h
        inputMgr.h
    )
    fips_dir(base)
    fips_files(inputMgrBase.cc inputMgrBase.h)
    fips_dir(touch)
    fips_files(
        gestureState.h
        panDetector.cc
        panDetector.h
        pinchDetector.cc
        pinchDetector.h
        tapDetector.cc
        tapDetector.h
        touchEvent.cc
        touchEvent.h
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
        fips_files(iosInputMgr.cc iosInputMgr.h)
    endif()
    if (FIPS_PNACL)
        fips_dir(pnacl)
        fips_files(pnaclInputMgr.cc pnaclInputMgr.h)
    endif()
    if (FIPS_MACOS OR FIPS_WINDOWS OR FIPS_LINUX)
        fips_dir(glfw)
        fips_files(glfwInputMgr.cc glfwInputMgr.h)
        fips_deps(glfw3)
    endif()
    fips_deps(Core Messaging Gfx Time)
fips_end_module()

