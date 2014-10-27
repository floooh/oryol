#-------------------------------------------------------------------------------
#   android.toolchain.cmake
#
#   NOTE: this script expects the Android SDK, NDK and a standalone
#   toolchain in the 'sdks' subdirectory as created by 
#   './oryol setup android'
#-------------------------------------------------------------------------------

message("Target Platform: Android")

set(ORYOL_PLATFORM ANDROID)
set(ORYOL_PLATFORM_NAME "android")
set(ORYOL_ANDROID 1)
set(ORYOL_POSIX 1)
set(ORYOL_OPENGL 1)
set(ORYOL_OPENGLES3 1)
set(ORYOL_PLATFORM_DEFINES "-DORYOL_ANDROID=1 -DORYOL_POSIX=1")

# verbose compile mode? (to check how headers and link libs are resolved)
if (ORYOL_COMPILE_VERBOSE)
    set(ORYOL_ANDROID_COMPILE_VERBOSE "-v")
    set(ORYOL_ANDROID_LINK_VERBOSE "-Wl,--verbose")
else()
    set(ORYOL_ANDROID_COMPILE_VERBOSE "")
    set(ORYOL_ANDROID_LINK_VERBOSE "")
endif()

# exceptions on/off?
if (ORYOL_EXCEPTIONS)
    message("C++ exceptions are enabled")
    set(ORYOL_ANDROID_EXCEPTION_FLAGS "")
else()
    message("C++ exceptions are disabled")
    set(ORYOL_ANDROID_EXCEPTION_FLAGS "-fno-exceptions")
endif()

# tweakable values (FIXME: convert to cmake options)
set(CMAKE_SYSTEM_PROCESSOR "arm")
set(ANDROID_API "android-19")
set(ANDROID_NDK_STL "gnu-libstdc++")
set(ANDROID_NDK_GCC_VERSION "4.8")
set(ANDROID_NDK_ARCH "armeabi-v7a")
set(ANDROID_NDK_NAME "android-ndk-r9d")
set(ANDROID_NDK_ABI "armeabi-v7a-hard")
set(ANDROID_NDK_ABI_EXT "arm-linux-androideabi")
set(ANDROID_NDK_GCC_PREFIX "arm-linux-androideabi")

# paths
if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin") 
    set(ANDROID_NDK_HOST "darwin-x86_64")
    set(ANDROID_NDK_ROOT "${CMAKE_CURRENT_LIST_DIR}/../sdks/osx/${ANDROID_NDK_NAME}")
    set(ANDROID_SDK_ROOT "${CMAKE_CURRENT_LIST_DIR}/../sdks/osx/android-sdk-macosx")
    set(ANDROID_NDK_EXE_EXT "")
    set(ANDROID_SDK_TOOL_EXT "")
elseif (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    set(ANDROID_NDK_HOST "linux-x86_64")
    set(ANDROID_NDK_ROOT "${CMAKE_CURRENT_LIST_DIR}/../sdks/linux/${ANDROID_NDK_NAME}")
    set(ANDROID_SDK_ROOT "${CMAKE_CURRENT_LIST_DIR}/../sdks/linux/android-sdk-linux")
    set(ANDROID_NDK_EXE_EXT "")
    set(ANDROID_SDK_TOOL_EXT "")
elseif (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    set(ANDROID_NDK_HOST "windows")
    set(ANDROID_NDK_ROOT "${CMAKE_CURRENT_LIST_DIR}/../sdks/windows/${ANDROID_NDK_NAME}")
    set(ANDROID_SDK_ROOT "${CMAKE_CURRENT_LIST_DIR}/../sdks/windows/android-sdk-windows")
    set(ANDROID_NDK_EXE_EXT ".exe")
    set(ANDROID_SDK_TOOL_EXT ".bat")
endif()

set(ANDROID_SDK_TOOL "${ANDROID_SDK_ROOT}/tools/android${ANDROID_SDK_TOOL_EXT}")
set(ANDROID_NDK_SYSROOT "${ANDROID_NDK_ROOT}/platforms/${ANDROID_API}/arch-arm")
set(ANDROID_NDK_TOOLCHAIN_BIN "${ANDROID_NDK_ROOT}/toolchains/${ANDROID_NDK_ABI_EXT}-${ANDROID_NDK_GCC_VERSION}/prebuilt/${ANDROID_NDK_HOST}/bin")

# CPU architecture dependent flags
set(ANDROID_NDK_ARCH_CFLAGS "-march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=hard -mthumb -mhard-float -D_NDK_MATH_NO_SOFTFP=1")
set(ANDROID_NDK_CMATHLIB "m_hard")

# STL dependent flags (FIXME: select based on ANDROID_NDK_STL)
set(ANDROID_NDK_STL_ROOT "${ANDROID_NDK_ROOT}/sources/cxx-stl/${ANDROID_NDK_STL}/${ANDROID_NDK_GCC_VERSION}")
set(ANDROID_NDK_STL_CXXFLAGS "-I${ANDROID_NDK_STL_ROOT}/include -I${ANDROID_NDK_STL_ROOT}/libs/${ANDROID_NDK_ABI}/include")
set(ANDROID_NDK_STL_LIBRARYPATH "${ANDROID_NDK_STL_ROOT}/libs/${ANDROID_NDK_ABI}")
set(ANDROID_NDK_STL_LDFLAGS "-lgnustl_static")

set(ANDROID_NDK_GLOBAL_CFLAGS "-fPIC -fno-strict-aliasing -ffunction-sections -funwind-tables -fstack-protector -no-canonical-prefixes")
set(ANDROID_NDK_CXX_WARN_FLAGS "-Wall -Wno-multichar -Wextra -Wno-unused-parameter -Wno-unknown-pragmas -Wno-ignored-qualifiers -Wno-long-long -Wno-overloaded-virtual")
set(ANDROID_NDK_C_WARN_FLAGS "-Wall -Wno-multichar -Wextra -Wno-unused-parameter -Wno-unknown-pragmas -Wno-ignored-qualifiers -Wno-long-long")

# get the gcc companion lib (FIXME: NOT NEEDED?)
#exec_program(
#    ${ANDROID_NDK_ROOT}/toolchains/${ANDROID_NDK_ABI_EXT}-${ANDROID_NDK_GCC_VERSION}/prebuilt/${ANDROID_NDK_HOST}/bin/${ANDROID_NDK_GCC_PREFIX}-gcc 
#    ARGS "-print-libgcc-file-name"
#    OUTPUT_VARIABLE ANDROID_NDK_GCC_COMPANIONLIBRARY
#    )
#get_filename_component(ANDROID_NDK_GCC_COMPANIONLIBRARY_PATH ${ANDROID_NDK_GCC_COMPANIONLIBRARY} PATH)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)
set(COMPILING ON)
SET(CMAKE_SKIP_RPATH ON)
set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_SKIP_COMPATIBILITY_TESTS 1)

# find the ant tool
find_program(ANDROID_ANT "ant")
if (ANDROID_ANT)
    message("ant tool found")
else()
    message(FATAL_ERROR "ant tool NOT FOUND (must be in path)!")
endif()

# disable compiler detection
include(CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER("${CMAKE_C_COMPILER}" GNU)
CMAKE_FORCE_CXX_COMPILER("${CMAKE_CXX_COMPILER}" GNU)

# define configurations
set(CMAKE_CONFIGURATION_TYPES Debug Release)

# standard libraries
set(CMAKE_C_STANDARD_LIBRARIES "-landroid -llog -lc -l${ANDROID_NDK_CMATHLIB} -lgcc")
set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_C_STANDARD_LIBRARIES} ${ANDROID_NDK_STL_LDFLAGS}")

# specify cross-compilers
set(CMAKE_C_COMPILER "${ANDROID_NDK_TOOLCHAIN_BIN}/${ANDROID_NDK_GCC_PREFIX}-gcc${ANDROID_NDK_EXE_EXT}" CACHE PATH "gcc" FORCE)
set(CMAKE_CXX_COMPILER "${ANDROID_NDK_TOOLCHAIN_BIN}/${ANDROID_NDK_GCC_PREFIX}-g++${ANDROID_NDK_EXE_EXT}" CACHE PATH "g++" FORCE)
set(CMAKE_AR "${ANDROID_NDK_TOOLCHAIN_BIN}/${ANDROID_NDK_GCC_PREFIX}-ar${ANDROID_NDK_EXE_EXT}" CACHE PATH "archive" FORCE)
set(CMAKE_LINKER "${ANDROID_NDK_TOOLCHAIN_BIN}/${ANDROID_NDK_GCC_PREFIX}-g++${ANDROID_NDK_EXE_EXT}" CACHE PATH "linker" FORCE)
set(CMAKE_RANLIB "${ANDROID_NDK_TOOLCHAIN_BIN}/${ANDROID_NDK_GCC_PREFIX}-ranlib${ANDROID_NDK_EXE_EXT}" CACHE PATH "ranlib" FORCE)

# only search for libraries and includes in the toolchain
set(CMAKE_FIND_ROOT_PATH ${ANDROID_NDK_SYSROOT})
set(CMAKE_SYSTEM_PROGRAM_PATH ${ANDROID_NDK_TOOLCHAIN_BIN})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(ANDROID_C_FLAGS "${ORYOL_ANDROID_COMPILE_VERBOSE} ${ANDROID_NDK_ARCH_CFLAGS} --sysroot=${ANDROID_NDK_SYSROOT} ${ANDROID_NDK_GLOBAL_CFLAGS} -DANDROID -Wa,--noexecstack -Wformat -Werror=format-security")
set(ANDROID_LD_FLAGS "-shared --sysroot=${ANDROID_NDK_SYSROOT} -L${ANDROID_NDK_STL_LIBRARYPATH} -no-canonical-prefixes -Wl,--fix-cortex-a8 -Wl,--no-warn-mismatch -Wl,--no-undefined -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now ${ORYOL_ANDROID_LINK_VERBOSE}")

# c++ compiler flags
set(CMAKE_CXX_FLAGS "${ANDROID_C_FLAGS} ${ORYOL_PLATFORM_DEFINES} -std=gnu++11 ${ANDROID_NDK_STL_CXXFLAGS} ${ORYOL_ANDROID_EXCEPTION_FLAGS} ${ANDROID_NDK_CXX_WARN_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -fomit-frame-pointer -funswitch-loops -finline-limit=300 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fno-omit-frame-pointer -g -D_DEBUG_ -D_DEBUG -DORYOL_DEBUG=1")

# c compiler flags
set(CMAKE_C_FLAGS "${ANDROID_C_FLAGS} ${ORYOL_PLATFORM_DEFINES} ${ANDROID_NDK_C_WARN_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE "-Os -fomit-frame-pointer -funswitch-loops -finline-limit=64 -DNDEBUG")
set(CMAKE_C_FLAGS_DEBUG "-O0 -fno-omit-frame-pointer -g -D_DEBUG_ -D_DEBUG -DORYOL_DEBUG=1")

# shared linker flags (native code on Android always lives in DLLs)
set(CMAKE_SHARED_LINKER_FLAGS "${ANDROID_LD_FLAGS} -pthread -dead_strip")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "")

# update cache variables for cmake gui
set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "Config Type" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING "Generic C++ Compiler Flags" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}" CACHE STRING "C++ Debug Compiler Flags" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "C++ Release Compiler Flags" FORCE)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "Generic C Compiler Flags" FORCE)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}" CACHE STRING "C Debug Compiler Flags" FORCE)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}" CACHE STRING "C Release Compiler Flags" FORCE)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}" CACHE STRING "Generic Linker Flags" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG}" CACHE STRING "Debug Linker Flags" FORCE)
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}" CACHE STRING "Release Linker Flags" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}" CACHE STRING "Generic Shared Linker Flags" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}" CACHE STRING "Debug Shared Linker Flags" FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}" CACHE STRING "Release Shared Linker Flags" FORCE)

# set the build type to use
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Compile Type" FORCE)
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release)
