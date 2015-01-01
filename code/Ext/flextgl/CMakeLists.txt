#
#   flextgl generated files
#
if (FIPS_LINUX OR FIPS_WINDOWS OR FIPS_MACOS)
    fips_begin_lib(flextgl)
        fips_files(flextGL.c flextGL.h)
    fips_end_lib()
endif()
