#-------------------------------------------------------------------------------
#   pnacl.toolchain.cmake
#   Oryol cmake toolchain file for cross-compiling to PNaCl.
#-------------------------------------------------------------------------------

message("Target Platform: PNaCl")

set(ORYOL_PLATFORM PNACL)
set(ORYOL_PLATFORM_NAME "pnacl")
set(ORYOL_PNACL 1)
set(ORYOL_POSIX 1)
set(ORYOL_PLATFORM_DEFINES "-DORYOL_PNACL=1 -DORYOL_POSIX=1")

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)
set(COMPILING on)

include(CMakeForceCompiler)

if (NOT NACL_SDK_ROOT)
    set(NACL_SDK_ROOT "$ENV{NACL_SDK_ROOT}")
endif()

if ("${NACL_SDK_ROOT}" STREQUAL "")
        message(FATAL_ERROR "Could not locate the NaCl toolchain directory! Either set the NACL_SDK_ROOT environment variable, or pass -DNACL_SDK_ROOT=xxx to CMake to explicitly specify the location of the compiler!")
endif()

# FIXME: this currently only works on Mac
set(NACL_TOOLCHAIN_ROOT "${NACL_SDK_ROOT}/toolchain/mac_pnacl")

# Normalize, convert Windows backslashes to forward slashes or CMake will crash.
get_filename_component(NACL_SDK_ROOT "${NACL_SDK_ROOT}" ABSOLUTE)

include(CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER("${CMAKE_C_COMPILER}" GNU)
CMAKE_FORCE_CXX_COMPILER("${CMAKE_CXX_COMPILER}" GNU)

# define configurations
set(CMAKE_CONFIGURATION_TYPES Debug Release)

# only search for libraries and includes in the toolchain
set(CMAKE_FIND_ROOT_PATH ${NACL_TOOLCHAIN_ROOT})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# specify cross-compilers
set(CMAKE_C_COMPILER "${NACL_TOOLCHAIN_ROOT}/bin/pnacl-clang" CACHE PATH "gcc" FORCE)
set(CMAKE_CXX_COMPILER "${NACL_TOOLCHAIN_ROOT}/bin/pnacl-clang++" CACHE PATH "g++" FORCE)
set(CMAKE_AR "${NACL_TOOLCHAIN_ROOT}/bin/pnacl-ar" CACHE PATH "archive" FORCE)
set(CMAKE_NM "${NACL_TOOLCHAIN_ROOT}/bin/pnacl-nm" CACHE PATH "nm" FORCE)
set(CMAKE_STRIP "${NACL_TOOLCHAIN_ROOT}/bin/pnacl-strip" CACHE PATH "strip" FORCE)
set(CMAKE_RANLIB "${NACL_TOOLCHAIN_ROOT}/bin/pnacl-ranlib" CACHE PATH "ranlib" FORCE)
set(CMAKE_LINKER "${NACL_TOOLCHAIN_ROOT}/bin/pnacl-clang++" CACHE PATH "linker" FORCE)

# compiler flags
set(CMAKE_CXX_FLAGS "${ORYOL_PLATFORM_DEFINES} -pthread -MMD -fno-exceptions -Wno-multichar -Wall -Wextra -Wno-unused-parameter -Wno-unknown-pragmas -Wno-ignored-qualifiers -Wno-long-long -Wno-overloaded-virtual -Wno-unused-volatile-lvalue -Wno-deprecated-writable-strings")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -D_DEBUG_ -D_DEBUG -DNACL_SDK_DEBUG")

set(CMAKE_C_FLAGS "${ORYOL_PLATFORM_DEFINES} -pthread -MMD -Wno-multichar -Wall -Wextra -Wno-unused-parameter -Wno-unknown-pragmas -Wno-ignored-qualifiers -Wno-long-long -Wno-overloaded-virtual -Wno-unused-volatile-lvalue  -Wno-deprecated-writable-strings")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_C_FLAGS_DEBUG "-O0 -D_DEBUG_ -D_DEBUG -g -DNACL_SDK_DEBUG")

set(CMAKE_EXE_LINKER_FLAGS "")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "")

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







