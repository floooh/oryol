#-------------------------------------------------------------------------------
#   oryol Synth module (chip-tune sound)
#-------------------------------------------------------------------------------

oryol_begin_module(Synth)
oryol_sources(. Core base shaders)
oryol_sources_osx(al)
oryol_sources_emscripten(al)
oryol_deps(Resource Core)
oryol_frameworks_osx(OpenAL)
oryol_end_module()

