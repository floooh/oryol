#
# minimal zlib for oryol
#
oryol_begin_lib(zlib)
    oryol_sources(.)
    add_definitions(-D_NO_FSEEKO)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
oryol_end_lib(zlib)
