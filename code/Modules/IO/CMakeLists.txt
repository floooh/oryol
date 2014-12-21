#-------------------------------------------------------------------------------
#   oryol IO module
#-------------------------------------------------------------------------------
fips_begin_module(IO)
fips_sources(. Core FS Stream)
fips_deps(Messaging Core)
fips_end_module()

fips_begin_unittest(IO)
fips_sources(UnitTests)
fips_deps(IO Messaging Core)
fips_end_unittest()
