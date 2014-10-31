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

# on MacOS, GLGETATTRIBLOCATION must always be false because the
# OpenGL3 Core Profile is used
#if (ORYOL_MACOS)
#    message("=> Forcing ORYOL_GL_USE_GETATTRIBLOCATION to off on MacOS")
#    set(ORYOL_GL_USE_GETATTRIBLOCATION OFF CACHE BOOL "Use glGetAttribLocation instead of glBindAttribLocation" FORCE)
#endif()
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

oryol_begin_module(Gfx)
oryol_sources(. Attrs Core Setup Types Util base gl)
oryol_sources_emscripten(emsc)
oryol_sources_android(egl)
oryol_sources_ios(ios)
oryol_sources_pnacl(pnacl)
if (ORYOL_MACOS OR ORYOL_WINDOWS OR ORYOL_LINUX)
    include_directories(${ORYOL_ROOT_DIR}/code/Ext/glfw/include)
    oryol_sources(glfw)
    oryol_deps(glfw3)
endif()
if (ORYOL_WINDOWS OR ORYOL_LINUX)
    oryol_deps(glew)
endif()
if (ORYOL_WINDOWS)
    oryol_deps(opengl32)
endif()
if (ORYOL_LINUX)
    oryol_deps(X11 Xrandr Xi Xxf86vm Xcursor GL)
endif()
if (ORYOL_ANDROID)
    oryol_deps(GLESv3 EGL)
endif()
oryol_deps(Resource Messaging IO Core)
oryol_end_module()

oryol_begin_unittest(Gfx)
oryol_sources(UnitTests UnitTests/shd)
oryol_deps(HTTP Gfx)
oryol_end_unittest()

