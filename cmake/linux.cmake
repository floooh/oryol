#-------------------------------------------------------------------------------
#   linux.cmake
#   Oryol cmake settings file for Linux target platform.
#-------------------------------------------------------------------------------

message("Target Platform: Linux")

set(ORYOL_PLATFORM Linux)
set(ORYOL_PLATFORM_NAME "linux")
set(ORYOL_LINUX 1)
set(ORYOL_POSIX 1)
set(ORYOL_PLATFORM_DEFINES "-DORYOL_LINUX=1 -DORYOL_POSIX=1")

# define configuration types
set(CMAKE_CONFIGURATION_TYPES Debug Release)

# C++ flags
set(CMAKE_CXX_FLAGS "${ORYOL_PLATFORM_DEFINES} -fno-exceptions -fno-rtti -fno-strict-aliasing -Wno-multichar -Wall -Wextra -Wno-unused-parameter -Wno-unknown-pragmas -Wno-ignored-qualifiers")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ftree-vectorize -msse3 -ffast-math -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -D_DEBUG_ -D_DEBUG -ggdb")

# C flags
set(CMAKE_C_FLAGS "${ORYOL_PLATFORM_DEFINES} -fno-strict-aliasing -Wno-multichar -Wall -Wextra -Wno-unused-parameter -Wno-unknown-pragmas -Wno-ignored-qualifiers")        
set(CMAKE_C_FLAGS_RELEASE "-O3 -ftree-vectorize -msse3 -ffast-math -DNDEBUG")
set(CMAKE_C_FLAGS_DEBUG "-O0 -D_DEBUG_ -D_DEBUG -ggdb")

# exe linker flags
set(CMAKE_EXE_LINKER_FLAGS "-dead_strip -lpthread -luuid -lrt")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "")

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

