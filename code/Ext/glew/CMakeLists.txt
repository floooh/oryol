#
# glew.lib for Oryol
#
if (ORYOL_WINDOWS OR ORYOL_LINUX)
oryol_begin_lib(glew)
    oryol_sources(. GL)
    include_directories(.)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE=1 -DGLEW_STATIC=1)
oryol_end_lib(glew)
endif()
