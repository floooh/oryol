#-------------------------------------------------------------------------------
#   oryol IO module
#-------------------------------------------------------------------------------
oryol_begin_module(IO)
oryol_sources(. Core FS Stream)
oryol_deps(Messaging Core)
oryol_end_module()

oryol_begin_unittest(IO)
oryol_sources(UnitTests)
oryol_deps(IO Messaging Core)
oryol_end_unittest()
