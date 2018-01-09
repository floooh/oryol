#-------------------------------------------------------------------------------
#   oryol Gfx module
#
#-------------------------------------------------------------------------------

fips_begin_module(Gfx)
    fips_vs_warning_level(3)
    fips_files(
        Gfx.cc Gfx.h
        GfxTypes.cc GfxTypes.h
        GfxConfig.h
    )
    fips_dir(private)
    fips_files(
        sokolGfxBackend.cc sokolGfxBackend.h
        displayMgrBase.cc displayMgrBase.h
        displayMgr.h
    )
    if (ORYOL_D3D11)
        fips_files(winDisplayMgr.cc winDisplayMgr.h)
        fips_files(d3d11DisplayMgr.cc d3d11DisplayMgr.h)
    endif()
    if (ORYOL_METAL)
        fips_files(mtlDisplayMgr.h mtlDisplayMgr.mm)
        fips_frameworks_osx(Metal QuartzCore)
    endif()
    if (FIPS_ANDROID)
        fips_files(eglDisplayMgr.cc eglDisplayMgr.h)
        fips_libs(GLESv3 EGL)
    endif()
    if (FIPS_EMSCRIPTEN)
        fips_files(emscDisplayMgr.cc emscDisplayMgr.h)
    endif()
    if (FIPS_IOS AND NOT ORYOL_METAL)
        fips_files(iosDisplayMgr.mm iosDisplayMgr.h)
    endif()
    if (ORYOL_OPENGL)
        if (FIPS_RASPBERRYPI)
            fips_files(eglDisplayMgr.cc eglDisplayMgr.h)
            fips_libs(GLESv2 EGL bcm_host)
        elseif (FIPS_MACOS OR FIPS_WINDOWS OR FIPS_LINUX)
            fips_files(glfwDisplayMgr.cc glfwDisplayMgr.h)
            fips_files(flextGL.c flextGL.h)
            fips_deps(glfw3)
        endif()
        if (FIPS_IOS)
            fips_frameworks_osx(OpenGLES GLKit)
        endif()
    endif()
    if (ORYOL_D3D11)
        fips_libs(d3d11)
    endif()
    fips_deps(Resource Core)
fips_end_module()
if (FIPS_MSVC)
    # forcing value to bool
    target_compile_options(Gfx PRIVATE "/wd4800")
endif()
