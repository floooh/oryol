#-------------------------------------------------------------------------------
#   oryol IO module
#-------------------------------------------------------------------------------
oryol_begin_module(IO)
oryol_sources(.)
oryol_deps(Core)
oryol_end_module()

oryol_begin_unittest(IO)
oryol_sources(UnitTests)
oryol_deps(Core IO)
oryol_end_unittest()
