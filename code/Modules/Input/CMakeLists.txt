#-------------------------------------------------------------------------------
#   oryol Input module
#-------------------------------------------------------------------------------
oryol_begin_module(Input)
oryol_sources(. Core base)
oryol_deps(Core Render)
if (ORYOL_MACOS OR ORYOL_WINDOWS OR ORYOL_LINUX)
    include_directories(${ORYOL_ROOT_DIR}/code/Ext/glfw/include)
    oryol_sources(glfw)
    oryol_deps(glfw3)
endif()
oryol_end_module()

