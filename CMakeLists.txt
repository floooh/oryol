#-------------------------------------------------------------------------------
#	oryol cmake root file
#
#	See BUILD.md for details how to build oryol.
#-------------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8)

# set the oryol root directory and the project directory first!
get_filename_component(ORYOL_ROOT_DIR "../oryol" ABSOLUTE)
get_filename_component(ORYOL_PROJECT_DIR "../oryol" ABSOLUTE)

# load oryol cmake macros
include("cmake/oryol.cmake")

# setup the oryol cmake build system
oryol_setup()

# define the project name
oryol_project(oryol)

# setup top-level header search paths
include_directories(code)

# add subdirectories
oryol_add_subdirectory(code/core)
oryol_add_subdirectory(code/test)




