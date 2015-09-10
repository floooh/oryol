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
if (ORYOL_GL_USE_GETATTRIBLOCATION)
    add_definitions(-DORYOL_GL_USE_GETATTRIBLOCATION=1)
else()
    add_definitions(-DORYOL_GL_USE_GETATTRIBLOCATION=0)    
endif()

endif() # ORYOL_OPENGL

fips_begin_module(Gfx)
    fips_vs_warning_level(3)
    fips_files(Gfx.cc Gfx.h)
    fips_generate(TYPE MessageProtocol FROM GfxProtocol.yml)
    fips_dir(Attrs)
    fips_files(DisplayAttrs.h IndexBufferAttrs.h TextureAttrs.h VertexBufferAttrs.h)
    fips_dir(Core)
    fips_files(
        displayMgrBase.cc displayMgrBase.h
        BlendState.h
        ClearState.cc ClearState.h
        DepthStencilState.h
        Enums.h
        PrimitiveGroup.h
        RasterizerState.h
        StencilState.h
        VertexLayout.cc VertexLayout.h
        UniformLayout.cc UniformLayout.h
        displayMgr.h
        renderer.h
        gfxPointers.h
        GfxConfig.h
    )
    fips_dir(Resource)
    fips_files(
        resourcePools.h
        drawStateBase.h drawStateBase.cc
        drawState.h
        drawStateFactoryBase.cc drawStateFactoryBase.h
        drawStateFactory.h
        meshBase.cc meshBase.h
        mesh.h
        meshFactory.h
        shaderBase.h
        shader.h
        shaderFactory.h
        textureBase.cc textureBase.h
        texture.h
        textureFactory.h
        textureBundleBase.h
        textureBundle.h
        textureBundleFactory.h
        gfxResourceContainerBase.cc gfxResourceContainerBase.h
        gfxResourceContainer.h 
        MeshLoaderBase.cc MeshLoaderBase.h
        TextureLoaderBase.cc TextureLoaderBase.h
    )
    fips_dir(Setup)
    fips_files(
        DrawStateSetup.cc DrawStateSetup.h
        GfxSetup.cc GfxSetup.h
        MeshSetup.cc MeshSetup.h
        ShaderSetup.cc ShaderSetup.h
        TextureSetup.cc TextureSetup.h
        TextureBundleSetup.cc TextureBundleSetup.h
    )
    if (ORYOL_OPENGL)
        fips_dir(gl)
        fips_files(
            glDebugOutput.cc glDebugOutput.h
            glExt.cc glExt.h
            glInfo.cc glInfo.h
            glDrawState.cc glDrawState.h
            glDrawStateFactory.cc glDrawStateFactory.h
            glMesh.cc glMesh.h
            glMeshFactory.cc glMeshFactory.h
            glShader.cc glShader.h
            glShaderFactory.cc glShaderFactory.h
            glRenderer.cc glRenderer.h
            glTextureBundle.cc glTextureBundle.h
            glTextureBundleFactory.cc glTextureBundleFactory.h
            glTexture.cc glTexture.h
            glTextureFactory.cc glTextureFactory.h
            glTypes.cc glTypes.h
            glVertexAttr.h
            gl_decl.h
            gl_impl.h
        )
    endif()
    if (ORYOL_D3D11 OR ORYOL_D3D12)
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
            d3d11Mesh.cc d3d11Mesh.h
            d3d11MeshFactory.cc d3d11MeshFactory.h
            d3d11Shader.cc d3d11Shader.h
            d3d11ShaderFactory.cc d3d11ShaderFactory.h
            d3d11Texture.cc d3d11Texture.h
            d3d11TextureFactory.cc d3d11TextureFactory.h
            d3d11TextureBundle.cc d3d11TextureBundle.h
            d3d11TextureBundleFactory.cc d3d11TextureBundleFactory.h
            d3d11DrawState.h d3d11DrawState.cc
            d3d11DrawStateFactory.h d3d11DrawStateFactory.cc
            d3d11Renderer.cc d3d11Renderer.h
        )
    endif()
    if (ORYOL_D3D12)
        fips_dir(d3d12)
        fips_files(
            d3d12_impl.h d3d12_decl.h
            d3d12Config.h
            d3d12ResourceAllocator.cc d3d12ResourceAllocator.h
            d3d12Types.cc d3d12Types.h
            d3d12DisplayMgr.cc d3d12DisplayMgr.h
            d3d12Mesh.cc d3d12Mesh.h
            d3d12MeshFactory.cc d3d12MeshFactory.h
            d3d12Shader.cc d3d12Shader.h
            d3d12ShaderFactory.cc d3d12ShaderFactory.h
            d3d12DrawState.cc d3d12DrawState.h
            d3d12DrawStateFactory.cc d3d12DrawStateFactory.h
            d3d12TextureFactory.cc d3d12TextureFactory.h
            d3d12Renderer.cc d3d12Renderer.h
        )
    endif()
    if (ORYOL_METAL)
        fips_dir(mtl)
        fips_files(
            mtl_decl.h mtl_impl.h
            mtlDisplayMgr.h mtlDisplayMgr.mm
            mtlTypes.h mtlTypes.mm
            mtlMesh.h mtlMesh.mm
            mtlMeshFactory.h mtlMeshFactory.mm
            mtlShader.h mtlShader.mm
            mtlShaderFactory.h mtlShaderFactory.mm
            mtlDrawState.h mtlDrawState.mm
            mtlDrawStateFactory.h mtlDrawStateFactory.mm
            mtlTexture.h mtlTexture.mm
            mtlTextureFactory.h mtlTextureFactory.mm
            mtlTextureBundle.h mtlTextureBundle.mm
            mtlTextureBundleFactory.h mtlTextureBundleFactory.mm
            mtlRenderer.h mtlRenderer.mm
        )
        fips_frameworks_osx(Cocoa Metal QuartzCore)
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
    if (FIPS_IOS)
        fips_dir(ios)
        fips_files(iosDisplayMgr.mm iosDisplayMgr.h)
    endif()
    if (FIPS_PNACL)
        fips_dir(pnacl)
        fips_files(pnaclDisplayMgr.cc pnaclDisplayMgr.h)
    endif()
    if (ORYOL_OPENGL)
        if (FIPS_MACOS OR FIPS_WINDOWS OR FIPS_LINUX)
            fips_dir(glfw)
            fips_files(glfwDisplayMgr.cc glfwDisplayMgr.h)
            fips_deps(glfw3 flextgl)
            if (FIPS_WINDOWS)
                fips_libs(opengl32)
            endif()
            if (FIPS_LINUX)
                # FIXME: should these go into the fips-glfw CMakeLists file?
                fips_libs(X11 Xrandr Xi Xinerama Xxf86vm Xcursor GL)
            endif()
            if (FIPS_OSX)
                fips_frameworks_osx(Cocoa CoreVideo OpenGL)
            endif()
        endif()
    endif()
    if (ORYOL_D3D11)
        fips_libs(d3d11)
    endif()
    if (ORYOL_D3D12)
        fips_libs(d3d12 dxgi)
    endif()
    fips_deps(Resource Messaging IO Core)
fips_end_module()

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

