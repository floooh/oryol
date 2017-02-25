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
    fips_files(Gfx.cc Gfx.h)
    fips_dir(Attrs)
    fips_files(
        DisplayAttrs.h
        IndexBufferAttrs.h
        TextureAttrs.h
        VertexBufferAttrs.h
        ImageDataAttrs.h
    )
    fips_dir(Core)
    fips_files(
        displayMgrBase.cc displayMgrBase.h
        BlendState.h
        PassState.h
        DrawState.h
        DepthStencilState.h
        SamplerState.h
        Enums.h
        PrimitiveGroup.h
        RasterizerState.h
        StencilState.h
        VertexLayout.cc VertexLayout.h
        UniformBlockLayout.cc UniformBlockLayout.h
        TextureBlockLayout.cc TextureBlockLayout.h
        displayMgr.h
        renderer.h
        gfxPointers.h
        GfxConfig.h
        GfxEvent.h
        GfxFrameInfo.h
    )
    fips_dir(Resource)
    fips_files(
        resourcePools.h
        resourceBase.h resourceBase.cc
        resource.h
        factory.h
        pipelineFactoryBase.cc pipelineFactoryBase.h
        renderPassFactoryBase.cc renderPassFactoryBase.h
        gfxResourceContainerBase.cc gfxResourceContainerBase.h
        gfxResourceContainer.h 
        MeshLoaderBase.cc MeshLoaderBase.h
        TextureLoaderBase.cc TextureLoaderBase.h
    )
    fips_dir(Setup)
    fips_files(
        PipelineSetup.cc PipelineSetup.h
        GfxSetup.cc GfxSetup.h
        MeshSetup.cc MeshSetup.h
        ShaderSetup.cc ShaderSetup.h
        TextureSetup.cc TextureSetup.h
        RenderPassSetup.cc RenderPassSetup.h
    )
    if (ORYOL_OPENGL)
        fips_dir(gl)
        fips_files(
            glCaps.cc glCaps.h
            glResource.cc glResource.h
            glPipelineFactory.cc glPipelineFactory.h
            glMeshFactory.cc glMeshFactory.h
            glShaderFactory.cc glShaderFactory.h
            glRenderer.cc glRenderer.h
            glTextureFactory.cc glTextureFactory.h
            glRenderPassFactory.cc glRenderPassFactory.h
            glTypes.cc glTypes.h
            glVertexAttr.h
            gl_decl.h
            gl_impl.h
        )
    endif()
    if (NOT FIPS_UWP AND (ORYOL_D3D11 OR ORYOL_D3D12))
        fips_dir(win)
        fips_files(
            win_decl.h
            winDisplayMgr.cc winDisplayMgr.h
            winInputDefs.h    
        )
    endif()
    if (ORYOL_D3D11)
        fips_dir(d3d11)
        fips_files(
            d3d11_impl.h d3d11_decl.h
            d3d11DisplayMgr.cc d3d11DisplayMgr.h
            d3d11Types.cc d3d11Types.h
            d3d11Resource.cc d3d11Resource.h
            d3d11MeshFactory.cc d3d11MeshFactory.h
            d3d11ShaderFactory.cc d3d11ShaderFactory.h
            d3d11TextureFactory.cc d3d11TextureFactory.h
            d3d11PipelineFactory.h d3d11PipelineFactory.cc
            d3d11Renderer.cc d3d11Renderer.h
        )
    endif()
    if (ORYOL_D3D12)
        fips_dir(d3d12)
        fips_files(
            d3d12_impl.h d3d12_decl.h
            d3d12Config.h
            d3d12ResAllocator.cc d3d12ResAllocator.h
            d3d12DescAllocator.cc d3d12DescAllocator.h
            d3d12SamplerCache.cc d3d12SamplerCache.h
            d3d12Types.cc d3d12Types.h
            d3d12DisplayMgr.cc d3d12DisplayMgr.h
            d3d12Resource.cc d3d12Resource.h
            d3d12MeshFactory.cc d3d12MeshFactory.h
            d3d12ShaderFactory.cc d3d12ShaderFactory.h
            d3d12PipelineFactory.cc d3d12PipelineFactory.h
            d3d12TextureFactory.cc d3d12TextureFactory.h
            d3d12Renderer.cc d3d12Renderer.h
            fasthash.h
        )
    endif()
    if (ORYOL_METAL)
        fips_dir(mtl)
        fips_files(
            mtl_decl.h mtl_impl.h
            mtlDisplayMgr.h mtlDisplayMgr.mm
            mtlTypes.h mtlTypes.mm
            mtlResource.h mtlResource.mm
            mtlMeshFactory.h mtlMeshFactory.mm
            mtlShaderFactory.h mtlShaderFactory.mm
            mtlPipelineFactory.h mtlPipelineFactory.mm
            mtlTextureFactory.h mtlTextureFactory.mm
            mtlRenderer.h mtlRenderer.mm
            mtlReleaseQueue.h mtlReleaseQueue.mm
        )
        fips_frameworks_osx(Metal QuartzCore)
    endif()
    if (FIPS_ANDROID)
        fips_dir(egl)
        fips_files(eglDisplayMgr.cc eglDisplayMgr.h)
        fips_libs(GLESv3 EGL)
    endif()
    if (FIPS_EMSCRIPTEN)
        fips_dir(emsc)
        fips_files(emscDisplayMgr.cc emscDisplayMgr.h)
    endif()
    if (FIPS_IOS AND NOT ORYOL_METAL)
        fips_dir(ios)
        fips_files(iosDisplayMgr.mm iosDisplayMgr.h)
    endif()
    if (FIPS_PNACL)
        fips_dir(pnacl)
        fips_files(pnaclDisplayMgr.cc pnaclDisplayMgr.h)
    endif()
    if (ORYOL_OPENGL)
        if (FIPS_RASPBERRYPI)
            fips_dir(egl)
            fips_files(eglDisplayMgr.cc eglDisplayMgr.h)
            fips_libs(GLESv2 EGL bcm_host)
        elseif (FIPS_MACOS OR FIPS_WINDOWS OR FIPS_LINUX)
            fips_dir(glfw)
            fips_files(glfwDisplayMgr.cc glfwDisplayMgr.h)
            fips_dir(flextgl)
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
    if (ORYOL_D3D12)
        fips_libs(d3d12 dxgi)
    endif()
    fips_deps(Resource Core)
fips_end_module()
if (FIPS_MSVC)
    # forcing value to bool
    target_compile_options(Gfx PRIVATE "/wd4800")
endif()

fips_begin_unittest(Gfx)
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
    oryol_shader(TestShaderLibrary.shd)
    fips_deps(HTTP Gfx Assets)
fips_end_unittest()

