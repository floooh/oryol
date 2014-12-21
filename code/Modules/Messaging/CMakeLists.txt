#-------------------------------------------------------------------------------
#   oryol Messaging module
#-------------------------------------------------------------------------------
fips_begin_module(Messaging)
fips_sources(.)
fips_deps(Core)
fips_end_module()

fips_begin_unittest(Messaging)
fips_sources(UnitTests)
fips_deps(Messaging Core)
fips_end_unittest()
