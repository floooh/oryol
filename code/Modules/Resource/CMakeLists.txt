#-------------------------------------------------------------------------------
#   oryol Resource module
#-------------------------------------------------------------------------------
fips_begin_module(Resource)
fips_sources(.)
fips_deps(Core)
fips_end_module()

fips_begin_unittest(Resource)
fips_sources(UnitTests)
fips_deps(Resource Core)
fips_end_unittest()
