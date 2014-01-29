#-------------------------------------------------------------------------------
#   oryol IO module
#-------------------------------------------------------------------------------
oryol_begin_module(IO)
oryol_sources(.)
oryol_deps(Core Messaging)
oryol_end_module()

oryol_begin_unittest(IO)
oryol_sources(UnitTests)
oryol_deps(IO Core Messaging)
oryol_end_unittest()
