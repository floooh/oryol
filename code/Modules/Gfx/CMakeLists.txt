#-------------------------------------------------------------------------------
#   oryol Gfx module
#
#   useful links:
#
#   https://developer.apple.com/library/prerelease/mac/documentation/Miscellaneous/Conceptual/MetalProgrammingGuide/WhatsNewinOSXandiOS/WhatsNewinOSXandiOS.html
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
        ClearState.h
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
    )
    fips_dir(Resource)
    fips_files(
        resourcePools.h
        drawStateBase.h drawStateBase.cc
        drawState.h
        drawStateFactoryBase.cc drawStateFactoryBase.h
        drawStateFactory.h
        mesh.h
        meshFactory.h
        programBundle.h
        programBundleFactory.h
        shader.h
        shaderFactory.h
        texture.h
        textureFactory.h
        meshBase.cc meshBase.h
        programBundleBase.h
        shaderBase.cc shaderBase.h
        textureBase.cc textureBase.h
        gfxResourceContainer.cc gfxResourceContainer.h
        MeshLoaderBase.cc MeshLoaderBase.h
        TextureLoaderBase.cc TextureLoaderBase.h
    )
    fips_dir(Setup)
    fips_files(
        DrawStateSetup.cc DrawStateSetup.h
        GfxSetup.cc GfxSetup.h
        MeshSetup.cc MeshSetup.h
        ProgramBundleSetup.cc ProgramBundleSetup.h
        ShaderSetup.cc ShaderSetup.h
        TextureSetup.cc TextureSetup.h
    )
    if (ORYOL_OPENGL)
        fips_dir(gl)
        fips_files(
            glDebugOutput.cc glDebugOutput.h
            glEnums.h
            glExt.cc glExt.h
            glInfo.cc glInfo.h
            glDrawState.cc glDrawState.h
            glDrawStateFactory.cc glDrawStateFactory.h
            glMesh.cc glMesh.h
            glMeshFactory.cc glMeshFactory.h
            glProgramBundle.cc glProgramBundle.h
            glProgramBundleFactory.cc glProgramBundleFactory.h
            glRenderer.cc glRenderer.h
            glShader.cc glShader.h
            glShaderFactory.cc glShaderFactory.h
            glTexture.cc glTexture.h
            glTextureFactory.cc glTextureFactory.h
            glTypes.cc glTypes.h
            glVertexAttr.h
            gl_decl.h
            gl_impl.h
        )
    endif()
    if (ORYOL_D3D11)
        fips_dir(d3d11)
        fips_files(
            d3d11_impl.h d3d11_decl.h
            d3d11DisplayMgr.cc d3d11DisplayMgr.h
            d3d11Enums.h
            d3d11Types.cc d3d11Types.h
            d3d11Mesh.cc d3d11Mesh.h
            d3d11MeshFactory.cc d3d11MeshFactory.h
            d3d11Shader.cc d3d11Shader.h
            d3d11ShaderFactory.cc d3d11ShaderFactory.h
            d3d11ProgramBundle.cc d3d11ProgramBundle.h
            d3d11ProgramBundleFactory.cc d3d11ProgramBundleFactory.h
            d3d11Texture.cc d3d11Texture.h
            d3d11TextureFactory.cc d3d11TextureFactory.h
            d3d11DrawState.h d3d11DrawState.cc
            d3d11DrawStateFactory.h d3d11DrawStateFactory.cc
            d3d11Renderer.cc d3d11Renderer.h
            d3d11InputDefs.h
        )
    endif()
    if (ORYOL_METAL)
        fips_dir(mtl)
        fips_files(
            cocoa.h cocoa.mm
            cocoaInputDefs.h
            mtl_decl.h mtl_impl.h
            mtlDisplayMgr.h mtlDisplayMgr.mm
            mtlTypes.h mtlTypes.mm
            mtlEnums.h
            mtlMesh.h mtlMesh.mm
            mtlMeshFactory.h mtlMeshFactory.mm
            mtlShaderFactory.h mtlShaderFactory.cc
            mtlProgramBundle.h mtlProgramBundle.mm
            mtlProgramBundleFactory.h mtlProgramBundleFactory.mm
            mtlTextureFactory.h mtlTextureFactory.cc
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
    # FIXME: hmm strange, why doesn't recursive dependency resolution work here
    # (have to explicitely link with Gfx)
    fips_deps(HTTP Gfx Assets)
fips_end_unittest()

