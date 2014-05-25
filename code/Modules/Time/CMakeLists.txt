#-------------------------------------------------------------------------------
#   oryol Time module
#-------------------------------------------------------------------------------
oryol_begin_module(Time)
oryol_sources(.)
oryol_deps(Core)
oryol_end_module()

oryol_begin_unittest(Time)
oryol_sources(UnitTests)
oryol_deps(Time Core)
oryol_end_unittest()
