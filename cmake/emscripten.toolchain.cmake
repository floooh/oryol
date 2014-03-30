#-------------------------------------------------------------------------------
#	emscripten.toolchain.cmake
#	Oryol cmake toolchain file for cross-compiling to emscripten.
#-------------------------------------------------------------------------------

message("Target Platform: emscripten")

set(ORYOL_PLATFORM EMSCRIPTEN)
set(ORYOL_PLATFORM_NAME "emsc")
set(ORYOL_EMSCRIPTEN 1)
set(ORYOL_POSIX 1)
set(ORYOL_OPENGL 1)
set(ORYOL_OPENGLES2 1)
set(ORYOL_PLATFORM_DEFINES " -DORYOL_EMSCRIPTEN=1 -DORYOL_POSIX=1")

# total memory is 128MB for main thread, and 16 MB for worker
# NOTE: USE_MEMORY_INIT_FILE has/had problems that the script is already starting but the MEM file isn't loaded yet(?)
# at least I'm having weird startup problems...
set(EMSCRIPTEN_TOTAL_MEMORY 134217728)
set(EMSCRIPTEN_TOTAL_MEMORY_WORKER 16777216)
set(EMSCRIPTEN_USE_MEMORY_INIT_FILE 0)
set(EMSCRIPTEN_USE_LTO 1)
set(EMSCRIPTEN_USE_CLOSURE 0)
set(EMSCRIPTEN_ASSERTIONS 0)
set(EMSCRIPTEN_BUILD_VERBOSE 0)
set(EMSCRIPTEN_OUTLINING_LIMIT 20000)
# set(EMSCRIPTEN_LLVM_TARGET_DEFS "-s TARGET_X86=1 -s TARGET_LE32=0 ")

message("EMSCRIPTEN_TOTAL_MEMORY: ${EMSCRIPTEN_TOTAL_MEMORY}")
message("EMSCRIPTEN_TOTAL_MEMORY_WORKER: ${EMSCRIPTEN_TOTAL_MEMORY_WORKER}")
message("EMSCRIPTEN_USE_MEMORY_INIT_FILE: ${EMSCRIPTEN_USE_MEMORY_INIT_FILE}")
message("EMSCRIPTEN_USE_LTO: ${EMSCRIPTEN_USE_LTO}")
message("EMSCRIPTEN_USE_CLOSURE: ${EMSCRIPTEN_USE_CLOSURE}")
message("EMSCRIPTEN_ASSERTIONS: ${EMSCRIPTEN_ASSERTIONS}")
message("EMSCRIPTEN_OUTLINING_LIMIT: ${EMSCRIPTEN_OUTLINING_LIMIT}")

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)
set(COMPILING on)
set(CMAKE_CROSSCOMPILING TRUE)

# Locate where the Emscripten compiler resides in relative to this toolchain file.
if ("${EMSCRIPTEN_ROOT_PATH}" STREQUAL "")
    get_filename_component(GUESS_EMSCRIPTEN_ROOT_PATH "${CMAKE_CURRENT_LIST_DIR}/../../emscripten" ABSOLUTE)
    if (EXISTS "${GUESS_EMSCRIPTEN_ROOT_PATH}/emranlib")
        set(EMSCRIPTEN_ROOT_PATH "${GUESS_EMSCRIPTEN_ROOT_PATH}")
    endif()
endif()

# If not found by above search, locate using the EMSCRIPTEN environment variable.
if ("${EMSCRIPTEN_ROOT_PATH}" STREQUAL "")
    set(EMSCRIPTEN_ROOT_PATH "$ENV{EMSCRIPTEN}")
endif()

# Abort if not found. 
if ("${EMSCRIPTEN_ROOT_PATH}" STREQUAL "")
    message(FATAL_ERROR "Could not locate the Emscripten compiler toolchain directory! Either set the EMSCRIPTEN environment variable, or pass -DEMSCRIPTEN_ROOT_PATH=xxx to CMake to explicitly specify the location of the compiler!")
endif()
message("Emscripten SDK: ${EMSCRIPTEN_ROOT_PATH}")

# Normalize, convert Windows backslashes to forward slashes or CMake will crash.
get_filename_component(EMSCRIPTEN_ROOT_PATH "${EMSCRIPTEN_ROOT_PATH}" ABSOLUTE)

# tool suffic (.bat on windows)
if (CMAKE_HOST_WIN32)
    set(EMCC_SUFFIX ".bat")
else()
    set(EMCC_SUFFIX "")
endif()

include(CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER("${CMAKE_C_COMPILER}" Clang)
CMAKE_FORCE_CXX_COMPILER("${CMAKE_CXX_COMPILER}" Clang)

# define configurations
set(CMAKE_CONFIGURATION_TYPES Debug Release)

# specify cross-compilers
set(CMAKE_C_COMPILER "${EMSCRIPTEN_ROOT_PATH}/emcc${EMCC_SUFFIX}" CACHE PATH "gcc" FORCE)
set(CMAKE_CXX_COMPILER "${EMSCRIPTEN_ROOT_PATH}/em++${EMCC_SUFFIX}" CACHE PATH "g++" FORCE)
set(CMAKE_AR "${EMSCRIPTEN_ROOT_PATH}/emar${EMCC_SUFFIX}" CACHE PATH "archive" FORCE)
set(CMAKE_LINKER "${EMSCRIPTEN_ROOT_PATH}/emcc${EMCC_SUFFIX}" CACHE PATH "linker" FORCE)
set(CMAKE_RANLIB "${EMSCRIPTEN_ROOT_PATH}/emranlib${EMCC_SUFFIX}" CACHE PATH "ranlib" FORCE)

# only search for libraries and includes in the toolchain
set(CMAKE_FIND_ROOT_PATH ${EMSCRIPTEN_ROOT_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_SYSTEM_INCLUDE_PATH "${EMSCRIPTEN_ROOT_PATH}/system/include")

# c++ compiler flags
set(CMAKE_CXX_FLAGS "${ORYOL_PLATFORM_DEFINES} -std=c++11 -stdlib=libstdc++ -fno-exceptions -fstrict-aliasing -Wall -Wno-warn-absolute-paths -Wno-multichar -Wextra -Wno-unused-parameter -Wno-unknown-pragmas -Wno-ignored-qualifiers -Wno-long-long -Wno-overloaded-virtual -Wno-deprecated-writable-strings -Wno-unused-volatile-lvalue")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O2 -g -D_DEBUG_ -D_DEBUG -DORYOL_DEBUG=1")

# c compiler flags
set(CMAKE_C_FLAGS "${ORYOL_PLATFORM_DEFINES} -fstrict-aliasing -Wall -Wno-warn-absolute-paths -Wextra -Wno-multichar -Wno-unused-parameter -Wno-unknown-pragmas -Wno-ignored-qualifiers -Wno-long-long -Wno-overloaded-virtual -Wno-deprecated-writable-strings -Wno-unused-volatile-lvalue")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_C_FLAGS_DEBUG "-O2 -g -D_DEBUG_ -D_DEBUG -DORYOL_DEBUG=1")

# linker flags
set(CMAKE_EXE_LINKER_FLAGS "--memory-init-file ${EMSCRIPTEN_USE_MEMORY_INIT_FILE} -s WARN_ON_UNDEFINED_SYMBOLS=1 -s TOTAL_MEMORY=${EMSCRIPTEN_TOTAL_MEMORY} -s DISABLE_EXCEPTION_CATCHING=1")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-O2 --llvm-lto ${EMSCRIPTEN_USE_LTO} -s VERBOSE=${EMSCRIPTEN_BUILD_VERBOSE} -s ASM_JS=1 -s ASSERTIONS=${EMSCRIPTEN_ASSERTIONS} -s OUTLINING_LIMIT=${EMSCRIPTEN_OUTLINING_LIMIT} --closure ${EMSCRIPTEN_USE_CLOSURE}")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-O2 -g -s ASM_JS=1 -s VERBOSE=${EMSCRIPTEN_BUILD_VERBOSE}")

# dynamic lib linker flags
set(CMAKE_SHARED_LINKER_FLAGS "-shared --memory-init-file 0 -s WARN_ON_UNDEFINED_SYMBOLS=1 -s TOTAL_MEMORY=${EMSCRIPTEN_TOTAL_MEMORY_WORKER} -s BUILD_AS_WORKER=1 -s EXPORTED_FUNCTIONS=[\\\"_dowork\\\"] -s DISABLE_EXCEPTION_CATCHING=1")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "-O2 --llvm-lto ${EMSCRIPTEN_USE_LTO} -s VERBOSE=${EMSCRIPTEN_BUILD_VERBOSE} -s ASM_JS=1 -s ASSERTIONS=${EMSCRIPTEN_ASSERTIONS} -s OUTLINING_LIMIT=${EMSCRIPTEN_OUTLINING_LIMIT} --closure ${EMSCRIPTEN_USE_CLOSURE}")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "-O2 -g -s ASM_JS=1 -s VERBOSE=${EMSCRIPTEN_BUILD_VERBOSE} --closure 0")

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

