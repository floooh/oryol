fips_begin_module(Dbg)
    fips_files(Dbg.cc Dbg.h)
    fips_dir(text)
    fips_files(debugFont.cc debugTextRenderer.cc debugTextRenderer.h)
    fips_dir(shaders)
    fips_generate(TYPE Shader FROM DebugShaders.shd)
    fips_deps(Core Gfx)
fips_end_module()

