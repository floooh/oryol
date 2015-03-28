fips_begin_app(TurboBadgerDemo windowed)
    fips_files(TurboBadgerDemo.cc)
    fips_dir(tbdemo)
    fips_files(Demo01.cc Demo.h
               ListWindow.cc ListWindow.h
               ResourceEditWindow.cc ResourceEditWindow.h)
    fips_deps(Gfx TBUI)
fips_end_app()
