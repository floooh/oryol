#-------------------------------------------------------------------------------
#   pnacl.toolchain.cmake
#   Oryol cmake toolchain file for cross-compiling to PNaCl.
#-------------------------------------------------------------------------------

message("Target Platform: PNaCl")

set(NACL_SDK_BUNDLE "pepper_canary")
if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    set(NACL_SDK_DIRNAME "sdks/win/nacl_sdk")
    set(NACL_TOOLCHAIN_DIRNAME "win_pnacl")
elseif (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin")
    set(NACL_SDK_DIRNAME "sdks/osx/nacl_sdk")
    set(NACL_TOOLCHAIN_DIRNAME "mac_pnacl")
elseif (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    set(NACL_SDK_DIRNAME "sdks/linux/nacl_sdk")
    set(NACL_TOOLCHAIN_DIRNAME "linux_pnacl")
endif()

set(ORYOL_PLATFORM PNACL)
set(ORYOL_PLATFORM_NAME "pnacl")
set(ORYOL_PNACL 1)
set(ORYOL_POSIX 1)
set(ORYOL_OPENGL 1)
set(ORYOL_OPENGLES2 1)
set(ORYOL_PLATFORM_DEFINES "-DORYOL_PNACL=1 -DORYOL_POSIX=1")

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)
set(COMPILING on)

# exceptions on/off?
if (ORYOL_EXCEPTIONS)
    message("C++ exceptions are enabled")
    set(ORYOL_NACL_EXCEPTION_FLAGS "")
else()
    message("C++ exceptions are disabled")
    set(ORYOL_NACL_EXCEPTION_FLAGS "-fno-exceptions")
endif()

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)
set(COMPILING on)
set(CMAKE_CROSSCOMPILING TRUE)

macro(find_nacl_sdk_root)
    get_filename_component(NACL_SDK_ROOT "${CMAKE_CURRENT_LIST_DIR}/../${NACL_SDK_DIRNAME}/${NACL_SDK_BUNDLE}" ABSOLUTE)
    if (NOT EXISTS "${NACL_SDK_ROOT}/README")
        message(FATAL_ERROR "Could not find NaCl SDK at ${NACL_SDK_DIRNAME}/${NACL_SDK_BUNDLE}! See BUILD.md for instructions to setup Oryol for NaCl development!")
    else()
        message("NaCl SDK found: ${NACL_SDK_ROOT}")
        set(NACL_SDK_ROOT ${NACL_SDK_ROOT} CACHE STRING "NaCl SDK location.")
    endif()
endmacro()

# find the NaCl SDK
find_nacl_sdk_root()
set(NACL_TOOLCHAIN_ROOT "${NACL_SDK_ROOT}/toolchain/${NACL_TOOLCHAIN_DIRNAME}")

# set paths to toolchain components
set(NACL_BIN "${NACL_TOOLCHAIN_ROOT}/bin")
set(NACL_INCLUDE "${NACL_SDK_ROOT}/include")
set(NACL_LIB "${NACL_SDK_ROOT}/lib/pnacl/Release")

message("NACL_BIN: ${NACL_BIN}")
message("NACL_INCLUDE: ${NACL_INCLUDE}")
message("NACL_LIB: ${NACL_LIB}")

# standard header and lib search paths
include_directories(${NACL_INCLUDE})
link_directories(${NACL_LIB})

# disable compiler detection
include(CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER("${CMAKE_C_COMPILER}" GNU)
CMAKE_FORCE_CXX_COMPILER("${CMAKE_CXX_COMPILER}" GNU)

# define configurations
set(CMAKE_CONFIGURATION_TYPES Debug Release)

# standard libraries
set(CMAKE_CXX_STANDARD_LIBRARIES "-lppapi_gles2 -lppapi_cpp -lppapi -lpthread")
set(CMAKE_C_STANDARD_LIBRARIES "lppapi_gles2 -lppapi -lpthread")

# specify cross-compilers
set(CMAKE_C_COMPILER "${NACL_BIN}/pnacl-clang" CACHE PATH "gcc" FORCE)
set(CMAKE_CXX_COMPILER "${NACL_BIN}/pnacl-clang++" CACHE PATH "g++" FORCE)
set(CMAKE_AR "${NACL_BIN}/pnacl-ar" CACHE PATH "archive" FORCE)
set(CMAKE_LINKER "${NACL_BIN}/pnacl-clang++" CACHE PATH "linker" FORCE)
set(CMAKE_RANLIB "${NACL_BIN}/pnacl-ranlib" CACHE PATH "ranlib" FORCE)

# only search for libraries and includes in the toolchain
set(CMAKE_FIND_ROOT_PATH ${NACL_TOOLCHAIN_ROOT}/usr)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_SYSTEM_INCLUDE_PATH "${NACL_TOOLCHAIN_ROOT}/usr/include")

# compiler flags
set(CMAKE_CXX_FLAGS "${ORYOL_PLATFORM_DEFINES} ${ORYOL_NACL_EXCEPTION_FLAGS} -std=c++11 -pthread -Wno-multichar -Wall -Wextra -Wno-unused-parameter -Wno-unknown-pragmas -Wno-ignored-qualifiers -Wno-long-long -Wno-overloaded-virtual -Wno-unused-volatile-lvalue -Wno-deprecated-writable-strings")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -D_DEBUG_ -D_DEBUG -DNACL_SDK_DEBUG -DORYOL_DEBUG=1")

set(CMAKE_C_FLAGS "${ORYOL_PLATFORM_DEFINES} -pthread -Wno-multichar -Wall -Wextra -Wno-unused-parameter -Wno-unknown-pragmas -Wno-ignored-qualifiers -Wno-long-long -Wno-overloaded-virtual -Wno-unused-volatile-lvalue  -Wno-deprecated-writable-strings")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_C_FLAGS_DEBUG "-O0 -D_DEBUG_ -D_DEBUG -g -DNACL_SDK_DEBUG -DORYOL_DEBUG=1")

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

# set the build type to use
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Compile Type" FORCE)
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release)


