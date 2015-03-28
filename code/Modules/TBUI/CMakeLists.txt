fips_begin_module(TBUI)
    fips_files(TBUI.h TBUI.cc)
    fips_dir(tb)
    fips_files(tb_config.h
               tbMgr.cc tbMgr.h
               tbOryolRootWidget.cc tbOryolRootWidget.h
               tbOryolRenderer.cc tbOryolRenderer.h
    )
    fips_deps(Gfx turbobadger)
fips_end_module()
