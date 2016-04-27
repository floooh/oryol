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

# helper macro to add a sample to the webpage
file(REMOVE ${FIPS_DEPLOY_DIR}/oryol-webpage/websamples.yml)
macro(oryol_add_web_sample name desc type image src)
    file(APPEND ${FIPS_DEPLOY_DIR}/oryol-webpage/websamples.yml "- name: ${name}\n  desc: ${desc}\n  type: ${type}\n  image: ${CMAKE_CURRENT_LIST_DIR}/${image}\n  src: ${src}\n")
endmacro()

fips_include_directories(code)
fips_include_directories(code/Modules)
fips_ide_group(Modules)
fips_add_subdirectory(code/Modules)
if (ORYOL_SAMPLES)
    fips_ide_group(Samples)
    fips_include_directories(code/Samples)
    # also find out-of-source generated headers
    fips_include_directories(${CMAKE_BINARY_DIR}/code/Samples)
    fips_add_subdirectory(code/Samples)
endif()
fips_finish()

