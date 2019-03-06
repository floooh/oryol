#-------------------------------------------------------------------------------
#   oryol Gfx module
#
#-------------------------------------------------------------------------------

fips_begin_module(Gfx)
    fips_vs_warning_level(3)
    fips_files(
        Gfx.cc Gfx.h
        GfxTypes.cc GfxTypes.h
    )
    fips_dir(private)
    fips_files(
        displayMgrBase.cc displayMgrBase.h
        displayMgr.h
    )
    fips_dir(private/sokol)
    fips_files(sokolGfxBackend.cc sokolGfxBackend.h sokol_gfx.h)
    if (ORYOL_METAL)
        fips_files(sokolImpl.mm)
    else()
        fips_files(sokolImpl.cc)
    endif()
    if (ORYOL_D3D11)
        fips_dir(private/d3d11)
        fips_files(winDisplayMgr.cc winDisplayMgr.h)
        fips_files(d3d11DisplayMgr.cc d3d11DisplayMgr.h)
        fips_libs(d3d11)
    elseif (ORYOL_METAL)
        fips_dir(private/metal)
        fips_files(mtlDisplayMgr.h mtlDisplayMgr.mm)
        fips_frameworks_osx(Metal QuartzCore)
    elseif (ORYOL_OPENGL)
        fips_dir(private/gl)
        if (FIPS_ANDROID)
            fips_files(eglDisplayMgr.cc eglDisplayMgr.h)
            fips_libs(GLESv3 EGL)
        elseif (FIPS_EMSCRIPTEN)
            fips_files(emscDisplayMgr.cc emscDisplayMgr.h)
        elseif (FIPS_IOS)
            fips_files(iosDisplayMgr.mm iosDisplayMgr.h)
            fips_frameworks_osx(OpenGLES GLKit)
        elseif (FIPS_RASPBERRYPI)
            fips_files(eglDisplayMgr.cc eglDisplayMgr.h)
            fips_libs(GLESv2 EGL bcm_host)
        elseif (FIPS_MACOS OR FIPS_WINDOWS OR FIPS_LINUX)
            fips_files(glfwDisplayMgr.cc glfwDisplayMgr.h)
            fips_files(flextGL.c flextGL.h)
            fips_deps(glfw3)
        endif()
    endif()
    fips_deps(Resource Core)
fips_end_module()
if (FIPS_MSVC)
    # forcing value to bool
    target_compile_options(Gfx PRIVATE "/wd4800")
endif()
