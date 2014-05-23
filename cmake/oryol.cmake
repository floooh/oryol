#-------------------------------------------------------------------------------
#   oryol.cmake
#   Public cmake wrapper functions for Oryol.
#-------------------------------------------------------------------------------

include("${ORYOL_ROOT_DIR}/cmake/oryol_private.cmake")
include("${ORYOL_ROOT_DIR}/cmake/oryol_unittests.cmake")
include("${ORYOL_ROOT_DIR}/cmake/oryol_android.cmake")
include("${ORYOL_ROOT_DIR}/cmake/oryol_osx.cmake")
include("${ORYOL_ROOT_DIR}/cmake/oryol_pnacl.cmake")
include("${ORYOL_ROOT_DIR}/cmake/oryol_generators.cmake")

#-------------------------------------------------------------------------------
#   define top-level options for the whole project
#-------------------------------------------------------------------------------
option(ORYOL_NO_ASSERTS_IN_RELEASE "Remove asserts in release-mode" OFF)
if (ORYOL_NACL)
    option(ORYOL_UNITTESTS "Enable unit tests" OFF)
else()
    option(ORYOL_UNITTESTS "Enable unit tests" ON)
endif()
option(ORYOL_UNITTESTS_RUN_AFTER_BUILD "Automatically run unit tests after building" OFF)
option(ORYOL_UNITTESTS_HEADLESS "If enabled don't run tests which require a display" OFF)
option(ORYOL_EXCEPTIONS "Enable C++ exceptions" OFF)
option(ORYOL_ALLOCATOR_DEBUG "Enable allocator debugging code (slow)" OFF)
option(ORYOL_SAMPLES "Compile sample programs" ON)
option(ORYOL_FORCE_NO_THREADS "Enable to simulate no support for std::thread" OFF)
option(ORYOL_COMPILE_VERBOSE "Enable very verbose compilation" OFF)

# turn some dependent options on/off
if (ORYOL_UNITTESTS)
    enable_testing()
    set(ORYOL_EXCEPTIONS ON CACHE BOOL "Enable C++ exceptions" FORCE)
else()
    set(ORYOL_EXCEPTIONS OFF CACHE BOOL "Enable C++ exceptions" FORCE)
endif()

#-------------------------------------------------------------------------------
#   oryol_setup()
#   Performs one-time initialization of the build system. Must be called
#   at the start of the root CMakeLists.txt file.
#
macro(oryol_setup)

    message("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

    if (ORYOL_ROOT_DIR)
        message("ORYOL_ROOT_DIR: ${ORYOL_ROOT_DIR}")
    else()
        message(FATAL_ERROR "Must specify absolute ORYOL_ROOT_DIR before calling oryol_setup()!")
    endif()
    if (ORYOL_PROJECT_DIR)
        message("ORYOL_PROJECT_DIR: ${ORYOL_PROJECT_DIR}")
    else()
        message(FATAL_ERROR "Must specify absolute ORYOL_PROJECT_DIR before calling oryol_setup()!")
    endif()

    # set host system variables
    set (ORYOL_HOST_WIN32 0)
    set (ORYOL_HOST_OSX 0)
    set (ORYOL_HOST_LINUX 0)
    if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
        set(ORYOL_HOST_WINDOWS 1)
        message("Oryol host system: Windows")
    elseif (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin")
        set(ORYOL_HOST_OSX 1)
        message("Oryol host system: OSX")
    elseif (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
        set(ORYOL_HOST_LINUX 1)
        message("Oryol host system: Linux")         
    endif()

    # manually include "toolchain" files for non-crosscompiling scenarios
    if (NOT CMAKE_TOOLCHAIN_FILE)
        if (WIN32)
            include(${ORYOL_ROOT_DIR}/cmake/windows.cmake)
        elseif (APPLE)
            include(${ORYOL_ROOT_DIR}/cmake/osx.cmake)
        else()
            include(${ORYOL_ROOT_DIR}/cmake/linux.cmake)
        endif()
    endif()
    message("ORYOL_PLATFORM: " ${ORYOL_PLATFORM})

    # setup standard link directories
    oryol_setup_link_directories()

    # setup the target group variable, used to group targets into folders in IDEs
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set(TARGET_GROUP "")
    
    # various global defines
    if (ORYOL_ALLOCATOR_DEBUG)
        add_definitions(-DORYOL_ALLOCATOR_DEBUG=1)
    else()
        add_definitions(-DORYOL_ALLOCATOR_DEBUG=0)
    endif()
    if (ORYOL_UNITTESTS)
        add_definitions(-DORYOL_UNITTESTS=1)
        if (ORYOL_UNITTESTS_HEADLESS)
            add_definitions(-DORYOL_UNITTESTS_HEADLESS=1)
        else()
            add_definitions(-DORYOL_UNITTESTS_HEADLESS=0)
        endif()
    else()
        add_definitions(-DORYOL_UNITTESTS=0)
    endif()
    if (ORYOL_OPENGL)
        add_definitions(-DORYOL_OPENGL=1)
        if (ORYOL_OPENGLES2)
            add_definitions(-DORYOL_OPENGLES2=1)
        else()
            add_definitions(-DORYOL_OPENGLES2=0)
        endif()
    else()
        add_definitions(-DORYOL_OPENGL=0)
        add_definitions(-DORYOL_OPENGLES2=0)
    endif()
    if (ORYOL_NO_ASSERTS_IN_RELEASE)
        add_definitions(-DORYOL_NO_ASSERT=1)
    else()
        add_definitions(-DORYOL_NO_ASSERT=0)
    endif()
    if (ORYOL_FORCE_NO_THREADS)
        add_definitions(-DORYOL_FORCE_NO_THREADS=1)
    else()
        add_definitions(-DORYOL_FORCE_NO_THREADS=0)
    endif()

    # GLM (math lib) definitions
    add_definitions(-DGLM_FORCE_RADIANS=1)
    
    # check whether python is installed
    find_program(PYTHON "python")
    if (PYTHON)
        message("PYTHON INTERPRETER FOUND")
    else()
        message("PYTHON INTERPRETER NOT FOUND, *.xml FILES WILL NOT BE CONVERTED!")
    endif()

    # clear the web samples config file
    oryol_begin_web_samples()

endmacro()

#-------------------------------------------------------------------------------
#   oryol_finish()
#   Call at the end of the root CMakeLists.txt file
#
macro(oryol_finish)
    oryol_end_web_samples()
endmacro()

#-------------------------------------------------------------------------------
#   oryol_group(group)
#   Define the IDE group name for the following targets. 
#
macro(oryol_group group)
    set(ORYOL_TARGET_GROUP ${group})
endmacro()

#-------------------------------------------------------------------------------
#   oryol_project(proj)
#   Starts a new project.
#
macro(oryol_project proj)
    project(${proj})
endmacro()

#-------------------------------------------------------------------------------
#   oryol_begin_module(module)
#   Begin defining an oryol module.
#
macro(oryol_begin_module name)
    message("Oryol Module: name=" ${name})
    oryol_reset(${name})
    set(CurModuleName ${name})
endmacro()

#-------------------------------------------------------------------------------
#   oryol_end_module(module)
#   End defining an oryol module, the interesting stuff happens here.
#
macro(oryol_end_module)
    
    # setup dependency tracker variables for this module, executable
    # targets use this to resolve their dependencies
    set_property(GLOBAL PROPERTY ${CurModuleName}_deps ${CurDependencies})
    set_property(GLOBAL PROPERTY ${CurModuleName}_libs ${CurLinkLibs})
    set_property(GLOBAL PROPERTY ${CurModuleName}_frameworks ${CurFrameworks})

    # handle XML generators (pre-target)
    if (CurXmlFiles)
        oryol_handle_generator_files_pretarget("${CurXmlFiles}")
    endif()

    # add library target
    add_library(${CurModuleName} ${CurSources})
    oryol_apply_target_group(${CurModuleName})

    # handle XML generators (post-target)
    if (CurXmlFiles)
        oryol_handle_generator_files_posttarget(${CurModuleName} "${CurXmlFiles}")
    endif()

endmacro()

#-------------------------------------------------------------------------------
#   oryol_begin_lib(name)
#   Begin defining a static link library
#
macro(oryol_begin_lib name)
    message("Library: name=" ${name})
    oryol_reset(${name})
    set(CurLibraryName ${name})
endmacro()

#-------------------------------------------------------------------------------
#   oryol_end_library(name)
#   End defining a static link library.
#
macro(oryol_end_lib)
    
    # add library target
    add_library(${CurLibraryName} ${CurSources})
    oryol_apply_target_group(${CurLibraryName})

endmacro()

#-------------------------------------------------------------------------------
#   oryol_begin_app(name type)
#   Begin an oryol command line app.
#   Type can be "windowed" or "cmdline", default is "cmdline".
#
macro(oryol_begin_app name type)
    if (${type} STREQUAL "windowed" OR ${type} STREQUAL "cmdline")
        oryol_reset(${name})
        set(CurAppName ${name})
        set(CurAppType ${type})
        message("Oryol App: name=" ${CurAppName} " type=" ${CurAppType})
    else()
        message(FATAL_ERROR "type must be \"windowed\" or \"cmdline\"!")
    endif()
endmacro()

#-------------------------------------------------------------------------------
#   oryol_end_app()
#   End defining an application.
#
macro(oryol_end_app)

    # add standard frameworks and libs
    if (ORYOL_OSX) 
        oryol_frameworks_osx(${ORYOL_OSX_STANDARD_FRAMEWORKS})
    endif()

    # setup dependency tracker variables for this module, executable
    # targets use this to resolve their dependencies
    set_property(GLOBAL PROPERTY ${CurAppName}_deps ${CurDependencies})
    set_property(GLOBAL PROPERTY ${CurAppName}_libs ${CurLinkLibs})
    set_property(GLOBAL PROPERTY ${CurAppName}_frameworks ${CurFrameworks})

    if (NOT CurSources)
        message(FATAL_ERROR "No sources in target: ${CurAppName} !!!")
    endif()

    # handle XML generators (pre-target)
    if (CurXmlFiles)
        oryol_handle_generator_files_pretarget("${CurXmlFiles}")
    endif()

    # add executable target
    if (${CurAppType} STREQUAL "windowed")
        # a windowed application 
        if (ORYOL_OSX OR ORYOL_IOS)
            add_executable(${CurAppName} MACOSX_BUNDLE ${CurSources})
            oryol_osx_add_target_properties(${CurAppName})
            oryol_copy_osx_dylib_files(${CurAppName} 1)
        elseif (ORYOL_WIN32 OR ORYOL_WIN64)
            add_executable(${CurAppName} WIN32 ${CurSources})
        elseif (ORYOL_ANDROID)
            add_library(${CurAppName} SHARED ${CurSources})
        else()
            add_executable(${CurAppName} ${CurSources})
        endif()
    else()
        # a command line application
        if (ORYOL_ANDROID)
            add_library(${CurAppName} SHARED ${CurSources})
        else()
            add_executable(${CurAppName} ${CurSources})
        endif()
        if (ORYOL_OSX OR ORYOL_IOS)
            oryol_copy_osx_dylib_files(${CurAppName} 0)
        endif()
    endif()
    oryol_apply_target_group(${CurAppName})

    # android specific stuff
    if (ORYOL_ANDROID)
        oryol_android_create_project(${CurAppName})
        oryol_android_postbuildstep(${CurAppName})
    endif()

    # handle XML generators (post-target)
    if (CurXmlFiles)
        oryol_handle_generator_files_posttarget(${CurAppName} "${CurXmlFiles}")
    endif()

    # PNaCl specific stuff
    if (ORYOL_PNACL)
        oryol_pnacl_create_wrapper(${CurAppName})
        oryol_pnacl_post_buildsteps(${CurAppName})
    endif()

    # add dependencies for target
    oryol_resolve_dependencies(${CurAppName})
    oryol_resolve_linklibs(${CurAppName})
    if (ORYOL_OSX OR ORYOL_IOS)
        oryol_osx_resolve_frameworks(${CurAppName})
    endif()

    # setup executable output directory and postfixes (_debug, etc...)
    oryol_exe_output_directory(${CurAppName})    
    oryol_config_postfixes_for_exe(${CurAppName})

endmacro()

#-------------------------------------------------------------------------------
#   oryol_deps(deps ...)
#   Add one or more dependencies to the current target.
#
macro(oryol_deps deps)
    foreach(dep ${ARGV})
        list(APPEND CurDependencies ${dep})
    endforeach()    
endmacro()

#-------------------------------------------------------------------------------
#   oryol_libs(libs ...)
#   Add one or more link libraries to the current target.
#
macro(oryol_libs libs)
    foreach(lib ${ARGV})
        list(APPEND CurLinkLibs ${lib})
    endforeach()
endmacro()

#-------------------------------------------------------------------------------
#   oryol_sources(dirs ...)
#   Parse one or more directories for sources and add them to the current
#   target.
#
macro(oryol_sources dirs)
    foreach (dir ${ARGV})
        # gather files
        file(GLOB src ${dir}/*.cc ${dir}/*.cpp ${dir}/*.c ${dir}/*.m ${dir}/*.mm ${dir}/*.h ${dir}/*.hh)
        file(GLOB xmls ${dir}/*.xml)
        file(GLOB shds ${dir}/*.shd)

        # determine group folder name
        string(REPLACE / \\ groupName ${dir})
        if (${dir} STREQUAL .)
            source_group("" FILES ${src} ${xmls} ${shds})
        else()
            source_group(${groupName} FILES ${src} ${xmls} ${shds})
        endif()

        # add generated source files
        foreach (xml ${xmls})
            string(REPLACE .xml .cc xmlSrc ${xml})
            string(REPLACE .xml .h xmlHdr ${xml})
            list(APPEND CurSources ${xmlSrc} ${xmlHdr})
            if (${dir} STREQUAL .)
                source_group("" FILES ${xmlSrc} ${xmlHdr})
            else()
                source_group(${groupName} FILES ${xmlSrc} ${xmlHdr})
            endif()
        endforeach()

        # add to global tracker variables
        list(APPEND CurSources ${src} ${xmls} ${shds})
        list(APPEND CurXmlFiles ${xmls})

        # remove duplicate sources 
        if (CurSources)
            list(REMOVE_DUPLICATES CurSources)
        endif()

    endforeach()
endmacro()

#-------------------------------------------------------------------------------
#   oryol_sources_posix(dirs ...)
#   Add POSIX specific sources.
#
macro(oryol_sources_posix dirs)
    if (ORYOL_POSIX)
        oryol_sources(${ARGV})
    endif()
endmacro()

#-------------------------------------------------------------------------------
#   oryol_sources_linux(dirs ...)
#   Add Linux specific sources.
#-------------------------------------------------------------------------------
macro(oryol_sources_linux dirs)
    if (ORYOL_LINUX)
        oryol_sources(${ARGV})
    endif()
endmacro()

#-------------------------------------------------------------------------------
#   oryol_sources_windows(dirs ...)
#   Add Windows specific sources.
#
macro(oryol_sources_windows dirs)
    if (ORYOL_WINDOWS)
        oryol_sources(${ARGV})
    endif()
endmacro()

#-------------------------------------------------------------------------------
#   oryol_sources_osx(dirs ...)
#   Add OSX specific sources (iOS or MacOS)
#-------------------------------------------------------------------------------
macro(oryol_sources_osx dirs)
    if (ORYOL_OSX)
        oryol_sources(${ARGV})
    endif()
endmacro()

#-------------------------------------------------------------------------------
#   oryol_sources_emscripten(dirs ...)
#   Add emscripten specific sources.
#-------------------------------------------------------------------------------
macro(oryol_sources_emscripten dirs)
    if (ORYOL_EMSCRIPTEN)
        oryol_sources(${ARGV})
    endif()
endmacro()

#-------------------------------------------------------------------------------
#   oryol_sources_pnacl(dirs ...)
#   Add PNACL specific sources.
#-------------------------------------------------------------------------------
macro(oryol_sources_pnacl dirs)
    if (ORYOL_PNACL)
        oryol_sources(${ARGV})
    endif()
endmacro()

#-------------------------------------------------------------------------------
#   oryol_sources_android(dirs ...)
#   Add Android specific sources.
#-------------------------------------------------------------------------------
macro(oryol_sources_android dirs)
    if (ORYOL_ANDROID)
        oryol_sources(${ARGV})
    endif()
endmacro()

#-------------------------------------------------------------------------------
#   oryol_sources_ios(dirs ...)
#   Add IOS specific sources.
#-------------------------------------------------------------------------------
macro(oryol_sources_ios dirs)
    if (ORYOL_IOS)
        oryol_sources(${ARGV})
    endif()
endmacro()

#-------------------------------------------------------------------------------
#   oryol_add_subdirectory(dir)
#
macro(oryol_add_subdirectory dir)
    add_subdirectory(${dir})
endmacro()

