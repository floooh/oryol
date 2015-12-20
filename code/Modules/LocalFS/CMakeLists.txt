#-------------------------------------------------------------------------------
#   Oryol LocalFS module
#-------------------------------------------------------------------------------
fips_begin_module(LocalFS)
    fips_vs_warning_level(3)
    fips_files(
        LocalFileSystem.cc LocalFileSystem.h
    )
    fips_deps(IO Messaging Core)
fips_end_module()

fips_begin_unittest(LocalFS)
    fips_vs_warning_level(3)
    fips_dir(UnitTests)
    fips_files(LocalFileSystemTest.cc)
    fips_deps(LocalFS)
fips_end_unittest()
