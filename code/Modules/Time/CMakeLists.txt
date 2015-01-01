#-------------------------------------------------------------------------------
#   oryol Time module
#-------------------------------------------------------------------------------
fips_begin_module(Time)
    fips_files(
        Clock.cc Clock.h
        Duration.h
        TimePoint.h
    )
    fips_deps(Core)
fips_end_module()

fips_begin_unittest(Time)
    fips_dir(UnitTests)
    fips_files(
        ClockTest.cc
        DurationTest.cc
        TimePointTest.cc
    )
    fips_deps(Time Core)
fips_end_unittest()
