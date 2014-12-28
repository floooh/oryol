#
#   flextgl generated files
#
if (FIPS_LINUX OR FIPS_WINDOWS OR FIPS_MACOS)
    fips_begin_lib(flextgl)
      fips_sources(.)
    fips_end_lib()
endif()
