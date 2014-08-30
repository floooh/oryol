#-------------------------------------------------------------------------------
#   oryol nanovg integration wrapper
#-------------------------------------------------------------------------------
add_definitions(-DNVG_NO_STDIO=1)
add_definitions(-DNVG_NO_STB_ASSERTS=1)

oryol_begin_module(NanoVG)
oryol_sources(. nanovg)
oryol_deps(Core IO Render)
oryol_end_module()

