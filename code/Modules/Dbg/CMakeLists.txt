fips_begin_module(Dbg)
    fips_files(Dbg.cc Dbg.h)
    fips_dir(text)
    fips_files(debugFont.cc debugTextRenderer.cc debugTextRenderer.h)
    fips_dir(shaders)
    fips_files(DebugShaders.py DebugShaders.shd)
    fips_deps(Core Gfx)
fips_end_module()

