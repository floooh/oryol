#-------------------------------------------------------------------------------
#   oryol nanovg integration wrapper
#-------------------------------------------------------------------------------
add_definitions(-DNVG_NO_STDIO=1)
add_definitions(-DNVG_NO_STB_ASSERTS=1)

fips_begin_module(NanoVG)
fips_sources(. nanovg)
fips_deps(Core IO Gfx)
fips_end_module()

