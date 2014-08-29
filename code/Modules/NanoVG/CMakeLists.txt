#-------------------------------------------------------------------------------
#   oryol nanovg integration wrapper
#-------------------------------------------------------------------------------
add_definitions(-DNANOVG_NO_STDIO=1)

oryol_begin_module(NanoVG)
oryol_sources(. nanovg)
oryol_deps(Core IO Render)
oryol_end_module()

