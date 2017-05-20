# absolute path to Oryol
set(ORYOL_DIR ${CMAKE_CURRENT_LIST_DIR})

if (FIPS_PNACL)
    message(FATAL_ERROR "PNaCl support has been removed from Oryol")
endif()
if (FIPS_UWP)
    message(FATAL_ERROR "UWP support has been removed from Oryol")
endif()

# cmake options
option(ORYOL_SAMPLES "Build Oryol samples" ON)
set(ORYOL_SAMPLE_URL "http://floooh.github.com/oryol/data/" CACHE STRING "Sample data URL")
option(ORYOL_DEBUG_SHADERS "Enable/disable debug info for shaders" OFF)
if (FIPS_MACOS OR FIPS_LINUX OR FIPS_ANDROID)
    option(ORYOL_USE_LIBCURL "Use libcurl instead of native APIs" ON)
else() 
    option(ORYOL_USE_LIBCURL "Use libcurl instead of native APIs" OFF)
endif()

if (ORYOL_USE_LIBCURL)
    add_definitions(-DORYOL_USE_LIBCURL=1)
endif()

# profiling enabled?
if (FIPS_PROFILING)
    add_definitions(-DORYOL_PROFILING=1)
endif()

# use Visual Leak Detector?
# see https://github.com/floooh/fips-vld
if (FIPS_USE_VLD)
    add_definitions(-DORYOL_USE_VLD=1)
endif()

# automatic reference counting on OSX/iOS?
if (FIPS_OSX_USE_ARC OR FIPS_IOS_USE_ARC)
    add_definitions(-DORYOL_USE_ARC=1)
endif()

# use OpenAL?
if (FIPS_OSX OR FIPS_EMSCRIPTEN)
    set(ORYOL_OPENAL 1)
else()
    set(ORYOL_OPENAL 0)
endif()

# use Metal on OSX/iOS?
if (FIPS_OSX)
    option(ORYOL_USE_METAL "Use Metal 3D API on OSX/iOS" OFF)
    if (ORYOL_USE_METAL)
        set(ORYOL_METAL 1)
    endif()
endif()

# use D3D11 on Windows?
if (FIPS_WINDOWS)
    set(d3d11_default OFF)
    option(ORYOL_USE_D3D11 "Use D3D11 3D API on Windows" ${d3d11_default})
    if (ORYOL_USE_D3D11)
        set(ORYOL_D3D11 1)
    endif()
endif()

# use OpenGL?
if (NOT ORYOL_METAL AND NOT ORYOL_D3D11)
    set(ORYOL_OPENGL 1)
    if (FIPS_RASPBERRYPI)
        set(ORYOL_OPENGLES2 1)
    elseif (FIPS_LINUX OR FIPS_MACOS OR FIPS_WINDOWS)
        set(ORYOL_OPENGL_CORE_PROFILE 1)
    elseif (FIPS_IOS)
        set(ORYOL_OPENGLES3 1)
    elseif (FIPS_EMSCRIPTEN)
        if (FIPS_EMSCRIPTEN_USE_WEBGL2)
            set(ORYOL_OPENGLES3 1)
        else()
            set(ORYOL_OPENGLES2 1)
        endif()
    elseif (FIPS_ANDROID) 
        set(ORYOL_OPENGLES3 1)
    elseif (FIPS_EMSCRIPTEN) 
        if (FIPS_EMSCRIPTEN_USE_WEBGL2)
            set(ORYOL_OPENGLES3 1)
        else()
            set(ORYOL_OPENGLES2 1)
        endif()
    endif()
endif()

# POSIX platform defines
if (FIPS_POSIX)
    add_definitions(-DORYOL_POSIX=1)
    if (FIPS_LINUX)
        add_definitions(-DORYOL_LINUX=1)
    endif()
    if (FIPS_OSX)
        add_definitions(-DORYOL_OSX=1)
        if (FIPS_MACOS)
            add_definitions(-DORYOL_MACOS=1)
        endif()
        if (FIPS_IOS)
            add_definitions(-DORYOL_IOS=1)
        endif()
    endif()
    if (FIPS_EMSCRIPTEN)
        add_definitions(-DORYOL_EMSCRIPTEN=1)
    endif()
    if (FIPS_ANDROID)
        add_definitions(-DORYOL_ANDROID=1)
    endif()
endif()

# Windows platform defines
if (FIPS_WINDOWS)
    add_definitions(-DORYOL_WINDOWS=1)
    if (FIPS_WIN32)
        add_definitions(-DORYOL_WIN32=1)
    endif()
    if (FIPS_WIN64)
        add_definitions(-DORYOL_WIN64=1)
    endif()
endif()

# OpenGL defines
if (ORYOL_OPENGL)
    add_definitions(-DORYOL_OPENGL=1)
    if (ORYOL_OPENGLES2)
        set(ORYOL_SLANG GLES)
        add_definitions(-DORYOL_OPENGLES2=1)
    endif()
    if (ORYOL_OPENGLES3)
        set(ORYOL_SLANG GLES)
        add_definitions(-DORYOL_OPENGLES3=1)
    endif()
    if (ORYOL_OPENGL_CORE_PROFILE)
        set(ORYOL_SLANG GLSL)
        add_definitions(-DORYOL_OPENGL_CORE_PROFILE=1)
    endif()
endif()

# D3D11 defines
if (ORYOL_D3D11)
    set(ORYOL_SLANG HLSL)
    add_definitions(-DORYOL_D3D11=1)
endif()

# Metal defines
if (ORYOL_METAL)
    set(ORYOL_SLANG MSL)
    add_definitions(-DORYOL_METAL=1)
endif()

# OpenAL defines
if (ORYOL_OPENAL)
    add_definitions(-DORYOL_OPENAL=1)
endif()

# RaspberryPi defines
if (FIPS_RASPBERRYPI)
    add_definitions(-DORYOL_RASPBERRYPI=1)
    include_directories(
        "/opt/vc/include" 
        "/opt/vc/include/interface/vcos/pthreads"
        "/opt/vc/include/interface/vmcs_host/linux"
    )
    link_directories("/opt/vc/lib")
endif()

# misc defines
if (FIPS_ALLOCATOR_DEBUG)
    add_definitions(-DORYOL_ALLOCATOR_DEBUG=1)
endif()
if (FIPS_UNITTESTS)
    add_definitions(-DORYOL_UNITTESTS=1)
    if (FIPS_UNITTESTS_HEADLESS)
        add_definitions(-DORYOL_UNITTESTS_HEADLESS=1)
    endif()
endif()
if (FIPS_NO_ASSERTS_IN_RELEASE)
    add_definitions(-DORYOL_NO_ASSERT=1)
endif()
if (FIPS_FORCE_NO_THREADS)
    add_definitions(-DORYOL_FORCE_NO_THREADS=1)
endif()
if (FIPS_EMSCRIPTEN)
    add_definitions(-DORYOL_SAMPLE_URL=\"http://localhost/../data/\") # HACK
else()
    add_definitions(-DORYOL_SAMPLE_URL=\"${ORYOL_SAMPLE_URL}\")
endif()

#-------------------------------------------------------------------------------
#   Wrap shader code generation
#
macro(oryol_shader shd)
    if (ORYOL_DEBUG_SHADERS)
        set(args "{debug: 'true', slang: '${ORYOL_SLANG}'}")
    else()
        set(args "{debug: 'false', slang: '${ORYOL_SLANG}'}")
    endif()
    fips_generate(TYPE Shader FROM ${shd} OUT_OF_SOURCE ARGS ${args})
endmacro()

#-------------------------------------------------------------------------------
#   Add a sample to the samples webpage 
#
file(REMOVE ${FIPS_DEPLOY_DIR}/oryol-webpage/websamples.yml)
macro(oryol_add_web_sample name desc type image src)
    file(APPEND ${FIPS_DEPLOY_DIR}/oryol-webpage/websamples.yml "- name: ${name}\n  desc: ${desc}\n  type: ${type}\n  image: ${CMAKE_CURRENT_LIST_DIR}/${image}\n  src: ${src}\n")
endmacro()
