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
        displayMgrBase.cc displayMgrBase.h
        displayMgr.h
        renderer.h
        gfxPointers.h
        resourcePools.h
        resourceBase.h resourceBase.cc
        resource.h
        gfxFactory.h
        gfxFactoryBase.h gfxFactoryBase.cc
        gfxResourceContainer.h gfxResourceContainer.cc 
    )
    if (ORYOL_OPENGL)
        fips_dir(private/gl)
        fips_files(
            glCaps.cc glCaps.h
            glResource.cc glResource.h
            glFactory.cc glFactory.h
            glRenderer.cc glRenderer.h
            glTypes.cc glTypes.h
            gl_decl.h
            gl_impl.h
        )
    endif()
    if (ORYOL_D3D11)
        fips_dir(private/win)
        fips_files(
            win_decl.h
            winDisplayMgr.cc winDisplayMgr.h
            winInputDefs.h    
        )
    endif()
    if (ORYOL_D3D11)
        fips_dir(private/d3d11)
        fips_files(
            d3d11_impl.h d3d11_decl.h
            d3d11DisplayMgr.cc d3d11DisplayMgr.h
            d3d11Types.cc d3d11Types.h
            d3d11Resource.cc d3d11Resource.h
            d3d11Factory.cc d3d11Factory.h
            d3d11Renderer.cc d3d11Renderer.h
        )
    endif()
    if (ORYOL_METAL)
        fips_dir(private/mtl)
        fips_files(
            mtl_decl.h mtl_impl.h
            mtlDisplayMgr.h mtlDisplayMgr.mm
            mtlTypes.h mtlTypes.mm
            mtlResource.h mtlResource.mm
            mtlFactory.h mtlFactory.mm
            mtlRenderer.h mtlRenderer.mm
        )
        fips_frameworks_osx(Metal QuartzCore)
    endif()
    if (FIPS_ANDROID)
        fips_dir(private/egl)
        fips_files(eglDisplayMgr.cc eglDisplayMgr.h)
        fips_libs(GLESv3 EGL)
    endif()
    if (FIPS_EMSCRIPTEN)
        fips_dir(private/emsc)
        fips_files(emscDisplayMgr.cc emscDisplayMgr.h)
    endif()
    if (FIPS_IOS AND NOT ORYOL_METAL)
        fips_dir(private/ios)
        fips_files(iosDisplayMgr.mm iosDisplayMgr.h)
    endif()
    if (ORYOL_OPENGL)
        if (FIPS_RASPBERRYPI)
            fips_dir(private/egl)
            fips_files(eglDisplayMgr.cc eglDisplayMgr.h)
            fips_libs(GLESv2 EGL bcm_host)
        elseif (FIPS_MACOS OR FIPS_WINDOWS OR FIPS_LINUX)
            fips_dir(private/glfw)
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

oryol_begin_unittest(Gfx)
    fips_vs_warning_level(3)
    fips_dir(UnitTests)
    fips_files(
        DDSLoadTest.cc
        MeshFactoryTest.cc
        MeshSetupTest.cc
        RenderEnumsTest.cc
        RenderSetupTest.cc
        TextureFactoryTest.cc
        TextureSetupTest.cc
        VertexLayoutTest.cc
        glTypesTest.cc
    )
    oryol_shader(TestShaderLibrary.glsl)
    fips_deps(HttpFS Gfx Assets)
oryol_end_unittest()

