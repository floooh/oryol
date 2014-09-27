#-------------------------------------------------------------------------------
#   oryol Input module
#-------------------------------------------------------------------------------
oryol_begin_module(Input)
oryol_sources(. Core base touch)
oryol_sources_emscripten(emsc)
oryol_sources_pnacl(pnacl)
oryol_sources_android(android)
oryol_sources_ios(ios)
if (ORYOL_MACOS OR ORYOL_WINDOWS OR ORYOL_LINUX)
    include_directories(${ORYOL_ROOT_DIR}/code/Ext/glfw/include)
    oryol_sources(glfw)
    oryol_deps(glfw3)
endif()
oryol_deps(Core Gfx Time)
oryol_end_module()

