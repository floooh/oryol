#-------------------------------------------------------------------------------
#   oryol Render module
#-------------------------------------------------------------------------------
oryol_begin_module(Render)
oryol_sources(. base gl)
if (ORYOL_OSX OR ORYOL_WINDOWS OR ORYOL_LINUX)
    include_directories(${ORYOL_ROOT_DIR}/code/Ext/glfw/include)
    oryol_sources(glfw)
endif()
oryol_deps(Resource Messaging Core)
oryol_end_module()

oryol_begin_unittest(Render)
oryol_sources(UnitTests)
oryol_deps(Render)
oryol_end_unittest()

