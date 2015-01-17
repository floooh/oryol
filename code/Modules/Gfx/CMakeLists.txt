#-------------------------------------------------------------------------------
#   oryol Gfx module
#-------------------------------------------------------------------------------

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

#
# Disable vertex_array_object even if the extension is available.
# Useful for debugging the non-VAO code.
#
option(ORYOL_GL_DISABLE_VERTEXARRAYOBJECT "Disable use of vertex_array_object extension" OFF)
if (ORYOL_GL_DISABLE_VERTEXARRAYOBJECT)
    add_definitions(-DORYOL_GL_DISABLE_VERTEXARRAYOBJECT=1)
else()
    add_definitions(-DORYOL_GL_DISABLE_VERTEXARRAYOBJECT=0)
endif()

fips_begin_module(Gfx)
    fips_files(Gfx.cc Gfx.h GfxProtocol.py)
    fips_dir(Attrs)
    fips_files(DisplayAttrs.h IndexBufferAttrs.h TextureAttrs.h VertexBufferAttrs.h)
    fips_dir(Core)
    fips_files(
        BlendState.h
        DepthStencilState.h
        Enums.h
        GfxId.cc GfxId.h
        PrimitiveGroup.h
        RasterizerState.h
        StencilState.h
        VertexComponent.h
        VertexLayout.cc VertexLayout.h
        displayMgr.h
        drawState.cc drawState.h
        drawStateFactory.cc drawStateFactory.h
        drawStatePool.h
        mesh.h
        meshFactory.h
        meshPool.h
        programBundle.h
        programBundleFactory.h
        programBundlePool.h
        renderer.h
        resourceMgr.cc resourceMgr.h
        shader.h
        shaderFactory.h
        shaderPool.h
        texture.h
        textureFactory.h
        texturePool.h
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
    fips_dir(Util)
    fips_files(
        MeshBuilder.cc MeshBuilder.h
        RawMeshLoader.cc RawMeshLoader.h
        ShapeBuilder.cc ShapeBuilder.h
        TextureLoader.h
        VertexWriter.cc VertexWriter.h
    )
    fips_dir(base)
    fips_files(
        displayMgrBase.cc displayMgrBase.h
        loaderBase.h
        meshBase.cc meshBase.h
        meshLoaderBase.cc meshLoaderBase.h
        programBundleBase.h
        renderLoaderFactory.h
        renderSimpleFactory.h
        shaderBase.cc shaderBase.h
        textureBase.cc textureBase.h
        textureLoaderBase.cc textureLoaderBase.h
    )
    fips_dir(gl)
    fips_files(
        glDebugOutput.cc glDebugOutput.h
        glEnums.h
        glExt.cc glExt.h
        glInfo.cc glInfo.h
        glMesh.cc glMesh.h
        glMeshFactory.cc glMeshFactory.h
        glProgramBundle.cc glProgramBundle.h
        glProgramBundleFactory.cc glProgramBundleFactory.h
        glRenderer.cc glRenderer.h
        glShader.cc glShader.h
        glShaderFactory.cc glShaderFactory.h
        glTexture.cc glTexture.h
        glTextureFactory.cc glTextureFactory.h
        glTextureLoader.cc glTextureLoader.h
        glTypes.cc glTypes.h
        glVertexAttr.h
        gl_decl.h
        gl_impl.h
    )
    if (FIPS_ANDROID)
        fips_dir(egl)
        fips_files(eglDisplayMgr.cc eglDisplayMgr.h)
        fips_deps(GLESv3 EGL)
    endif()
    if (FIPS_EMSCRIPTEN)
        fips_dir(emsc)
        fips_files(emscDisplayMgr.cc emscDisplayMgr.h)
    endif()
    if (FIPS_IOS)
        fips_dir(ios)
        fips_files(iosDisplayMgr.cc iosDisplayMgr.h)
    endif()
    if (FIPS_PNACL)
        fips_dir(pnacl)
        fips_files(pnaclDisplayMgr.cc pnaclDisplayMgr.h)
    endif()
    if (FIPS_MACOS OR FIPS_WINDOWS OR FIPS_LINUX)
        fips_dir(glfw)
        fips_files(glfwDisplayMgr.cc glfwDisplayMgr.h)
        fips_deps(glfw3 flextgl)
        if (FIPS_WINDOWS)
            fips_deps(opengl32)
        endif()
        if (FIPS_LINUX)
            # FIXME: should these go into the fips-glfw CMakeLists file?
            fips_deps(X11 Xrandr Xi Xinerama Xxf86vm Xcursor GL)
        endif()
    endif()
    fips_deps(Resource Messaging IO Core)
fips_end_module()

fips_begin_unittest(Gfx)
    fips_dir(UnitTests)
    fips_files(
        DDSLoadTest.cc
        MeshBuilderTest.cc
        MeshFactoryTest.cc
        MeshSetupTest.cc
        RenderEnumsTest.cc
        RenderSetupTest.cc
        ShapeBuilderTest.cc
        TextureFactoryTest.cc
        TextureSetupTest.cc
        VertexLayoutTest.cc
        VertexWriterTest.cc
        glTypesTest.cc
    )
    fips_dir(UnitTests/shd)
    fips_files(test.shd)
    fips_deps(HTTP Gfx)
fips_end_unittest()

