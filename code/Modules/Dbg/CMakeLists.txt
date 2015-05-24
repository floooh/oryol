fips_begin_module(Dbg)
    fips_vs_warning_level(3)
    fips_files(Dbg.cc Dbg.h)
    oryol_shader(DebugShaders.shd)
    fips_dir(text)
    fips_files(debugFont.cc debugTextRenderer.cc debugTextRenderer.h)
    fips_deps(Core Gfx)
fips_end_module()

