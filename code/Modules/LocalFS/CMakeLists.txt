#-------------------------------------------------------------------------------
#   Oryol LocalFS module
#-------------------------------------------------------------------------------
fips_begin_module(LocalFS)
    fips_vs_warning_level(3)
    if (FIPS_MSVC)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    endif()
    fips_files(
        LocalFileSystem.cc LocalFileSystem.h
    )
    fips_dir(whereami)
    fips_files(whereami_oryol.cc whereami.h)
    if (FIPS_POSIX OR FIPS_WINDOWS)
        fips_dir(posix)
        fips_files(posixFSWrapper.cc posixFSWrapper.h)
    else()
        fips_dir(dummy)
        fips_files(dummyFSWrapper.cc dummyFSWrapper.h)
    endif()
    fips_dir(Core)
    fips_files(fsWrapper.h)
    fips_deps(IO Messaging Core)
fips_end_module()

fips_begin_unittest(LocalFS)
    fips_vs_warning_level(3)
    fips_dir(UnitTests)
    fips_files(
        LocalFileSystemTest.cc
        FSWrapperTest.cc
    )
    fips_deps(LocalFS)
fips_end_unittest()
