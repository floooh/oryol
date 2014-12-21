#-------------------------------------------------------------------------------
#   oryol Time module
#-------------------------------------------------------------------------------
fips_begin_module(Time)
fips_sources(.)
fips_deps(Core)
fips_end_module()

fips_begin_unittest(Time)
fips_sources(UnitTests)
fips_deps(Time Core)
fips_end_unittest()
