#-------------------------------------------------------------------------------
#   oryol Synth module (chip-tune sound)
#-------------------------------------------------------------------------------

fips_begin_module(Synth)
fips_sources(. Core base shaders)
fips_sources_osx(al)
fips_sources_emscripten(al)
fips_deps(Resource Core)
fips_end_module()

