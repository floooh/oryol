# port of soloud tedsid sample
if (NOT FIPS_PNACL)
fips_begin_app(SoloudTedSid windowed)
    fips_vs_warning_level(3)
    fips_files(
        SoloudTedSid.cc
    )
    fips_deps(Gfx IMUI IO HTTP soloud)
    oryol_add_web_sample(SoloudTedSid "Soloud integration" "emscripten" SoloudTedSid.jpg "SoloudTedSid/SoloudTedSid.cc")
fips_end_app()
endif()
