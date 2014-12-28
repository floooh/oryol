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
fips_sources(. Attrs Core Setup Types Util base gl)
fips_sources_emscripten(emsc)
fips_sources_android(egl)
fips_sources_ios(ios)
fips_sources_pnacl(pnacl)
if (FIPS_MACOS OR FIPS_WINDOWS OR FIPS_LINUX)
    fips_sources(glfw)
    fips_deps(glfw3)
    fips_deps(flextgl)
endif()
if (FIPS_WINDOWS)
    fips_deps(opengl32)
endif()
if (FIPS_LINUX)
    fips_deps(X11 Xrandr Xi Xxf86vm Xcursor GL)
endif()
if (FIPS_ANDROID)
    fips_deps(GLESv3 EGL)
endif()
fips_deps(Resource Messaging IO Core)
fips_end_module()

fips_begin_unittest(Gfx)
fips_sources(UnitTests UnitTests/shd)
fips_deps(HTTP Gfx)
fips_end_unittest()

