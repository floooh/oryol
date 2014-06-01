#-------------------------------------------------------------------------------
#   oryol Render module
#-------------------------------------------------------------------------------

# 
# This option forces the use of glGetAttribLocation() instead of
# glBindAttribLocation() to bind vertex components to shader 
# vertex attributes. This must be used on platforms where
# GL_MAX_VERTEX_ATTRS is less then 16 (the only platform with this
# restriction so far seems to be the Raspberry Pi). This option
# is mutually exclusive with vertex array objects.
#
option(ORYOL_USE_GLGETATTRIBLOCATION "Use glGetAttribLocation instead of glBindAttribLocation" OFF)

# on MacOS, GLGETATTRIBLOCATION must always be false because the
# OpenGL3 Core Profile is used
if (ORYOL_MACOS)
    message("=> Forcing ORYOL_USE_GLGETATTRIBLOCATION to off on MacOS")
    set(ORYOL_USE_GLGETATTRIBLOCATION OFF CACHE BOOL "Use glGetAttribLocation instead of glBindAttribLocation" FORCE)
endif()    
if (ORYOL_USE_GLGETATTRIBLOCATION)
    add_definitions(-DORYOL_USE_GLGETATTRIBLOCATION=1)
else()
    add_definitions(-DORYOL_USE_GLGETATTRIBLOCATION=0)    
endif()

oryol_begin_module(Render)
oryol_sources(. Attrs Core Setup Types Util base gl)
oryol_sources_emscripten(egl)
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
oryol_deps(Resource HTTP Messaging IO Core)
oryol_end_module()

oryol_begin_unittest(Render)
oryol_sources(UnitTests UnitTests/shd)
oryol_deps(Render)
oryol_end_unittest()

