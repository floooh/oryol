#-------------------------------------------------------------------------------
#   oryol Resource module
#-------------------------------------------------------------------------------
oryol_begin_module(Resource)
oryol_sources(.)
oryol_deps(Core)
oryol_end_module()

oryol_begin_unittest(Resource)
oryol_sources(UnitTests)
oryol_deps(Resource Core)
oryol_end_unittest()
