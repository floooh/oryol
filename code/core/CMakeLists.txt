#-------------------------------------------------------------------------------
#   oryol core module
#-------------------------------------------------------------------------------
oryol_begin_module(core)
oryol_sources(. threading string)
oryol_sources_posix(posix)
oryol_end_module()

oryol_begin_unittest(core)
oryol_sources(unittests)
oryol_deps(core)
oryol_end_unittest()
