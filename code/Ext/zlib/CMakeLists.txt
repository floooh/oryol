#
# minimal zlib for oryol
#
fips_begin_lib(zlib)
    fips_files(
        adler32.c
        compress.c
        crc32.c crc32.h
        deflate.c deflate.h
        infback.c 
        inffast.c inffast.h
        inffixed.h
        inflate.c inflate.h
        inftrees.c inftrees.h
        trees.c trees.h
        uncompr.c
        zconf.h
        zlib.h
        zutil.c zutil.h
    )
    add_definitions(-D_NO_FSEEKO)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
fips_end_lib(zlib)
