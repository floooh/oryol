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
    if (NOT FIPS_UWP)
        fips_files(whereami_oryol.cc whereami.h)
    endif()
    # web-platforms don't have a LocalFS implementation
    if (FIPS_EMSCRIPTEN OR FIPS_PNACL)
        fips_dir(dummy)
        fips_files(dummyFSWrapper.cc dummyFSWrapper.h)
    else()
        fips_dir(posix)
        fips_files(posixFSWrapper.cc posixFSWrapper.h)
    endif()
    fips_dir(Core)
    fips_files(fsWrapper.h)
    fips_deps(IO Core)
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
