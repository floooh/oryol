#-------------------------------------------------------------------------------
#	osx.cmake
#	Oryol cmake settings file for OSX target platform.
#	(C) 2013 A.Weissflog
#-------------------------------------------------------------------------------

message("Target Platform: OSX")

set(ORYOL_PLATFORM OSX)
set(ORYOL_PLATFORM_NAME "osx")
set(ORYOL_MACOS 1)
set(ORYOL_OSX 1)
set(ORYOL_POSIX 1)
set(ORYOL_OPENGL 1)
set(ORYOL_PLATFORM_DEFINES "-DORYOL_MACOS=1 -DORYOL_OSX=1 -DORYOL_POSIX=1")

# define configuration types
set(CMAKE_CONFIGURATION_TYPES Debug Release)

# define standard frame works that are always linked
set(ORYOL_OSX_STANDARD_FRAMEWORKS Foundation IOKit OpenGL Cocoa CoreVideo)

# need to set some flags directly as Xcode attributes
set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")

if (ORYOL_EXCEPTIONS)
    message("C++ exceptions are enabled")
    set(CMAKE_XCODE_ATTRIBUTE_GCC_ENABLE_CPP_EXCEPTIONS "YES")
else()
    message("C++ exceptions are disabled")
    set(CMAKE_XCODE_ATTRIBUTE_GCC_ENABLE_CPP_EXCEPTIONS "NO")
endif()

# compiler flags
set(CMAKE_CXX_FLAGS "${ORYOL_PLATFORM_DEFINES} -fstrict-aliasing -Wno-multichar -Wall -Wextra -Wno-unused-parameter -Wno-unknown-pragmas -Wno-ignored-qualifiers -Wno-long-long -Wno-overloaded-virtual -Wno-unused-volatile-lvalue -Wno-deprecated-writable-strings")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -msse3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -D_DEBUG_ -D_DEBUG -DORYOL_DEBUG=1 -ggdb")

set(CMAKE_C_FLAGS "${ORYOL_PLATFORM_DEFINES} -fstrict-aliasing -Wno-multichar -Wall -Wextra -Wno-unused-parameter -Wno-unknown-pragmas -Wno-ignored-qualifiers -Wno-long-long -Wno-overloaded-virtual -Wno-unused-volatile-lvalue  -Wno-deprecated-writable-strings")
set(CMAKE_C_FLAGS_RELEASE "-O3 -msse3 -DNDEBUG")
set(CMAKE_C_FLAGS_DEBUG "-O0 -D_DEBUG_ -D_DEBUG -DORYOL_DEBUG=1 -g")        	

set(CMAKE_EXE_LINKER_FLAGS "-ObjC -dead_strip -lpthread")
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
