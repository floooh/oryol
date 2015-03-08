#-------------------------------------------------------------------------------
#	oryol cmake root file
#
#	See BUILD.md for details how to build oryol.
#-------------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8)

get_filename_component(FIPS_ROOT_DIR "../fips" ABSOLUTE)
include("${FIPS_ROOT_DIR}/cmake/fips.cmake")

option(ORYOL_SAMPLES "Build Oryol samples" ON)

fips_setup(PROJECT oryol)
fips_include_directories(code)
fips_include_directories(code/Modules)
fips_include_directories(code/Ext)
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

