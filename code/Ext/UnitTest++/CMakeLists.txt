#
#   integrate UnitTest++ into Oryol
#
if (FIPS_UNITTESTS)

    # define UnitTest++ link lib
    fips_begin_lib(unittestpp)
        fips_sources(src)
        fips_sources_posix(src/Posix)
        fips_sources_windows(src/Win32)
    fips_end_lib()

    # add the UnitTest++ unit test itself
    # FIXME: hmm these don't currently run on OSX, WHY?
    # fips_begin_unittest(unittestpp)
    #    fips_sources(src/tests)
    #    fips_deps(core unittestpp)
    # fips_end_unittest()
endif()
