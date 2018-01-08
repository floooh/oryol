#-------------------------------------------------------------------------------
#   oryol Gfx module
#
#-------------------------------------------------------------------------------

if (ORYOL_OPENGL)
# 
# This option forces the use of glGetAttribLocation() instead of
# glBindAttribLocation() to bind vertex components to shader 
# vertex attributes. This must be used on platforms where
# GL_MAX_VERTEX_ATTRS is less then 16 (the only platform with this
# restriction so far seems to be the Raspberry Pi). This option
# is mutually exclusive with vertex array objects.
#
option(ORYOL_GL_USE_GETATTRIBLOCATION "Use glGetAttribLocation instead of glBindAttribLocation" OFF)
if (ORYOL_GL_USE_GETATTRIBLOCATION OR FIPS_RASPBERRYPI)
    add_definitions(-DORYOL_GL_USE_GETATTRIBLOCATION=1)
else()
    add_definitions(-DORYOL_GL_USE_GETATTRIBLOCATION=0)    
endif()

endif() # ORYOL_OPENGL

fips_begin_module(Gfx)
    fips_vs_warning_level(3)
    fips_files(
        Gfx.cc Gfx.h
        GfxTypes.cc GfxTypes.h
        GfxConfig.h
        MeshLoaderBase.cc MeshLoaderBase.h
        TextureLoaderBase.cc TextureLoaderBase.h
    )
    fips_dir(private)
    fips_files(
        sokolGfxBackend.cc sokolGfxBackend.h
        displayMgrBase.cc displayMgrBase.h
        displayMgr.h
        gfxResourceContainer.h gfxResourceContainer.cc 
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
            fips_dir(private/flextgl)
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
