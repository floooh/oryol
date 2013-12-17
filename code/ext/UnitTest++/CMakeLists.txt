#
#   integrate UnitTest++ into Oryol
#
if (ORYOL_UNITTESTS)

    # define UnitTest++ link lib
    oryol_begin_lib(unittestpp)
        oryol_sources(src)
        oryol_sources_posix(src/Posix)
        oryol_sources_windows(src/Win32)
    oryol_end_lib()

    # add the UnitTest++ unit test itself
    # FIXME: hmm these don't currently run on OSX, WHY?
    # oryol_begin_unittest(unittestpp)
    #    oryol_sources(src/tests)
    #    oryol_deps(core unittestpp)
    # oryol_end_unittest()
endif()