#-------------------------------------------------------------------------------
#   oryol nanovg integration wrapper
#-------------------------------------------------------------------------------
if (ORYOL_OPENGL)

add_definitions(-DNVG_NO_STDIO=1)
add_definitions(-DNVG_NO_STB_ASSERTS=1)

fips_begin_module(NanoVG)
    fips_vs_warning_level(3)
    fips_vs_disable_warnings(4996)   # 'this function or variable may be unsafe' for fopen in fontstash.h
    fips_files(NanoVG.cc NanoVG.h NanoVGWrapper.h)
    fips_dir(nanovg)
    fips_files(
        fontstash.h
        nanovg.c nanovg.h
        nanovg_gl.h
        stb_image.h
        stb_truetype.h
    )
    fips_deps(Core IO Gfx)
fips_end_module()

endif()
