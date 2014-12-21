# platform identification

# POSIX platforms
if (FIPS_POSIX)
    set(ORYOL_OPENGL 1)
    add_definitions(-DORYOL_POSIX=1)
    if (FIPS_LINUX)
        set(ORYOL_OPENGL_CORE_PROFILE 1)
        add_definitions(-DORYOL_LINUX=1)
    endif()
    if (FIPS_OSX)
        add_definitions(-DORYOL_OSX=1)
        if (FIPS_MACOS)
            set(ORYOL_OPENGL_CORE_PROFILE 1)
            add_definitions(-DORYOL_MACOS=1)
        endif()
        if (FIPS_IOS)
            set(ORYOL_OPENGLES2 1)
            add_definitions(-DORYOL_IOS=1)
        endif()
    endif()
    if (FIPS_PNACL)
        set(ORYOL_OPENGLES2 1)
        add_definitions(-DORYOL_PNACL=1)
    endif()
    if (FIPS_EMSCRIPTEN)
        set(ORYOL_OPENGLES2 1)
        add_definitions(-DORYOL_EMSCRIPTEN=1)
    endif()
    if (FIPS_ANDROID)
        set(ORYOL_OPENGLES3 1)
        add_definitions(-DORYOL_ANDROID=1)
    endif()
endif()

# Windows platforms
if (FIPS_WINDOWS)
    set(ORYOL_OPENGL 1)
    set(ORYOL_OPENGL_CORE_PROFILE 1)
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
        add_definitions(-DORYOL_OPENGLES2=1)
    endif()
    if (ORYOL_OPENGLES3)
        add_definitions(-DORYOL_OPENGLES3=1)
    endif()
    if (ORYOL_OPENGL_CORE_PROFILE)
        add_definitions(-DORYOL_OPENGL_CORE_PROFILE=1)
    endif()
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
if (FIPS_EMSCRIPTEN OR FIPS_PNACL)
    add_definitions(-DORYOL_SAMPLE_URL=\"http://localhost/\")
else()
    add_definitions(-DORYOL_SAMPLE_URL=\"${ORYOL_SAMPLE_URL}\")
endif()
add_definitions(-DGLM_FORCE_RADIANS=1)

#-------------------------------------------------------------------------------
#   Clear and start websamples description file.
#
macro(oryol_begin_web_samples)
    file(WRITE ${FIPS_DEPLOY_DIR}/webpage/websamples.json "{\n  \"samples\": [\n")
endmacro()

#-------------------------------------------------------------------------------
#   Add a sample file to the web samples description file
#
macro(oryol_add_web_sample name desc type image src)
    file(APPEND ${FIPS_DEPLOY_DIR}/webpage/websamples.json "    { \"name\": \"${name}\", \"desc\": \"${desc}\", \"type\": \"${type}\", \"image\": \"${CMAKE_CURRENT_LIST_DIR}/${image}\", \"src\": \"${src}\" },\n")
endmacro()

#-------------------------------------------------------------------------------
#   Finish the web samples description file
#
macro(oryol_end_web_samples)
    file(APPEND ${FIPS_DEPLOY_DIR}/webpage/websamples.json "    { \"name\": \"__end__\" }\n  ]\n}\n")
endmacro()
