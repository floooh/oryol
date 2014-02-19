#-------------------------------------------------------------------------------
#   oryol core module
#-------------------------------------------------------------------------------
oryol_begin_module(Core)
oryol_sources(. Threading String Memory Containers)
oryol_sources_posix(posix)
oryol_sources_windows(windows)
oryol_deps(ConvertUTF)
oryol_end_module()

oryol_begin_unittest(Core)
oryol_sources(UnitTests)
oryol_deps(Core)
oryol_end_unittest()
