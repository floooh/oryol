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
    fips_dir(private)
    fips_files(fsWrapper.h)
    fips_dir(private/whereami)
    if (NOT FIPS_EMSCRIPTEN)
        fips_files(whereami_oryol.cc whereami.h)
    endif()
    # web-platforms don't have a LocalFS implementation
    if (FIPS_EMSCRIPTEN)
        fips_dir(private/dummy)
        fips_files(dummyFSWrapper.cc dummyFSWrapper.h)
    else()
        fips_dir(private/posix)
        fips_files(posixFSWrapper.cc posixFSWrapper.h)
    endif()
    fips_deps(IO Core)
fips_end_module()

oryol_begin_unittest(LocalFS)
    fips_vs_warning_level(3)
    fips_dir(UnitTests)
    fips_files(
        LocalFileSystemTest.cc
        FSWrapperTest.cc
    )
    fips_deps(LocalFS)
oryol_end_unittest()
