#-------------------------------------------------------------------------------
#   oryol core module
#-------------------------------------------------------------------------------
fips_begin_module(Core)
fips_sources(. Threading String Memory Containers)
fips_sources_posix(posix)
fips_sources_windows(windows)
fips_sources_android(android)
fips_sources_ios(ios)
fips_sources_pnacl(pnacl)
fips_deps(ConvertUTF)
if (FIPS_ANDROID)
    fips_deps(android_native)
endif()
fips_end_module()

fips_begin_unittest(Core)
fips_sources(UnitTests)
fips_deps(Core)
fips_end_unittest()

