#-------------------------------------------------------------------------------
#   oryol Render module
#-------------------------------------------------------------------------------
oryol_begin_module(Render)
oryol_sources(. base)
oryol_deps(Resource Core)
oryol_end_module()

oryol_begin_unittest(Render)
oryol_sources(UnitTests)
oryol_deps(Render Resource Core)
oryol_end_unittest()

