fips_begin_app(TurboBadgerDemo windowed)
    fips_vs_warning_level(3)
    fips_vs_disable_warnings(4267)   # converting from 64-bit to 32-bit values, this happens often in Turbobadger source
    fips_files(
        TurboBadgerDemo.cc
        TBDemoWindows.cc TBDemoWindows.h
    )
    fips_dir(tbdemo)
    fips_deps(Gfx HTTP Dbg TBUI)
    oryol_add_web_sample(TurboBadgerDemo "Turbobadger UI integration" "emscripten,pnacl" TurboBadgerDemo.jpg "TurboBadgerDemo/TurboBadgerDemo.cc")
fips_end_app()
