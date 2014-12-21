#
# minimal zlib for oryol
#
fips_begin_lib(zlib)
    fips_sources(.)
    add_definitions(-D_NO_FSEEKO)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
fips_end_lib(zlib)
