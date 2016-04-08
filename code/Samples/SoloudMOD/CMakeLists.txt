if (NOT FIPS_PNACL)
fips_begin_app(SoloudMOD windowed)
    fips_vs_warning_level(3)
    fips_files(
        SoloudMOD.cc
    )
    fips_deps(Gfx IMUI IO HTTP soloud libmodplug)
    oryol_add_web_sample(SoloudMOD "Soloud MOD sample" "emscripten" SoloudMOD.jpg "SoloudMOD/SoloudMOD.cc")
fips_end_app()
endif()
