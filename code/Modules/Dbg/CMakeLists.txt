fips_begin_module(Dbg)
    fips_vs_warning_level(3)
    fips_files(Dbg.cc Dbg.h)
    fips_dir(private)
    fips_files(debugFont.cc debugTextRenderer.cc debugTextRenderer.h)
    oryol_shader(DebugShaders.glsl)
    fips_deps(Core Gfx)
fips_end_module()

