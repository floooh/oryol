#-------------------------------------------------------------------------------
#	oryol cmake root file
#
#	See BUILD.md for details how to build oryol.
#-------------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8)

get_filename_component(FIPS_ROOT_DIR "../fips" ABSOLUTE)
include("${FIPS_ROOT_DIR}/cmake/fips.cmake")

option(ORYOL_SAMPLES "Build Oryol samples" ON)

include_directories(code)
include_directories(code/Modules)
include_directories(code/Ext)
include_directories(code/Ext/glfw/include)

fips_setup()
fips_project(oryol)
fips_add_subdirectory(code/Hello)
fips_ide_group(Modules)
fips_add_subdirectory(code/Modules)
fips_ide_group(Ext)
fips_add_subdirectory(code/Ext)
if (ORYOL_SAMPLES)
   fips_ide_group(Samples)
   fips_add_subdirectory(code/Samples)
endif()
fips_finish()

