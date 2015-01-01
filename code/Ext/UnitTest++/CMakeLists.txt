#
#   integrate UnitTest++ into Oryol
#
if (FIPS_UNITTESTS)

    # define UnitTest++ link lib
    fips_begin_lib(unittestpp)
        fips_dir(src)
        fips_files(
            AssertException.cpp AssertException.h
            CheckMacros.h
            Checks.cpp Checks.h
            Config.h
            CurrentTest.cpp CurrentTest.h
            DeferredTestReporter.cpp DeferredTestReporter.h
            DeferredTestResult.cpp DeferredTestResult.h
            ExecuteTest.h
            MemoryOutStream.cpp MemoryOutStream.h
            ReportAssert.cpp ReportAssert.h
            Test.cpp Test.h
            TestDetails.cpp TestDetails.h
            TestList.cpp TestList.h
            TestMacros.h
            TestReporter.cpp TestReporter.h
            TestReporterStdout.cpp TestReporterStdout.h
            TestResults.cpp TestResults.h
            TestRunner.cpp TestRunner.h
            TestSuite.h
            TimeConstraint.cpp TimeConstraint.h
            TimeHelpers.h
            UnitTest++.h
            XmlTestReporter.cpp XmlTestReporter.h
        )
        if (FIPS_POSIX)
            fips_dir(src/Posix)
            fips_files(
                SignalTranslator.cpp SignalTranslator.h
                TimeHelpers.cpp TimeHelpers.h
            )
        endif()
        if (FIPS_WINDOWS)
            fips_dir(src/Win32)
            fips_files(TimeHelpers.cpp TimeHelpers.h)
        endif()
    fips_end_lib()

    # add the UnitTest++ unit test itself
    # FIXME: hmm these don't currently run on OSX, WHY?
    # fips_begin_unittest(unittestpp)
    #    fips_sources(src/tests)
    #    fips_deps(core unittestpp)
    # fips_end_unittest()
endif()
