#
#   external libraries
#
fips_add_subdirectory(UnitTest++)
fips_add_subdirectory(ConvertUTF)
fips_add_subdirectory(zlib)
if (FIPS_ANDROID)
  fips_add_subdirectory(android_native)
endif()
if (FIPS_LINUX OR FIPS_WINDOWS OR FIPS_MACOS)
  fips_add_subdirectory(flextgl)
endif()
