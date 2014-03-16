#
# glew.lib for Oryol
#
if (ORYOL_WINDOWS OR ORYOL_LINUX OR ORYOL_OSX)
oryol_begin_lib(glew)
    oryol_sources(. GL)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE=1 -DGLEW_STATIC=1)
oryol_end_lib(glew)
endif()
