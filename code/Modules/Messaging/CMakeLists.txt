#-------------------------------------------------------------------------------
#   oryol Messaging module
#-------------------------------------------------------------------------------
oryol_begin_module(Messaging)
oryol_sources(.)
oryol_deps(Core)
oryol_end_module()

oryol_begin_unittest(Messaging)
oryol_sources(UnitTests)
oryol_deps(Messaging Core)
oryol_end_unittest()
