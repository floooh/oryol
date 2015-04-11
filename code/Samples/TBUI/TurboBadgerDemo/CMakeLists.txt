fips_begin_app(TurboBadgerDemo windowed)
    fips_files(
        TurboBadgerDemo.cc
        TBDemoWindows.cc TBDemoWindows.h
    )
    fips_dir(tbdemo)
    fips_deps(Gfx HTTP Dbg TBUI)
    oryol_add_web_sample(TurboBadgerDemo "Turbobadger UI integration" "emscripten,pnacl" TurboBadgerDemo.jpg "TBUI/TurboBadgerDemo/TurboBadgerDemo.cc")
fips_end_app()
