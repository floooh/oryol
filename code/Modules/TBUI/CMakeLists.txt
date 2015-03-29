fips_begin_module(TBUI)
    fips_files(TBUI.h TBUI.cc
               TBUISetup.h
    )
    fips_dir(tb)
    fips_files(tb_config.h
               tbMgr.cc tbMgr.h
               tbOryolRootWidget.cc tbOryolRootWidget.h
               tbOryolRenderer.cc tbOryolRenderer.h
               tbOryolFile.cc tbOryolFile.h
               tbResource.h
               TBUIResourceContainer.cc TBUIResourceContainer.h
    )
    fips_deps(Gfx turbobadger)
fips_end_module()
