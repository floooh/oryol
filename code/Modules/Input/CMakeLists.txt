#-------------------------------------------------------------------------------
#   oryol Input module
#-------------------------------------------------------------------------------
fips_begin_module(Input)
fips_sources(. Core base touch)
fips_sources_emscripten(emsc)
fips_sources_pnacl(pnacl)
fips_sources_android(android)
fips_sources_ios(ios)
if (FIPS_MACOS OR FIPS_WINDOWS OR FIPS_LINUX)
    fips_sources(glfw)
    fips_deps(glfw3)
endif()
fips_deps(Core Gfx Time)
fips_end_module()

