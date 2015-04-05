fips_begin_app(TurboBadgerDemo windowed)
    fips_files(
        TurboBadgerDemo.cc
        MainWindow.cc MainWindow.h
    )
    fips_dir(tbdemo)
    fips_deps(Gfx HTTP Dbg TBUI)
fips_end_app()
