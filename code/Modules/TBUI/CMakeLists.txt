fips_begin_module(TBUI)
    fips_files(
        TBUI.h TBUI.cc
        TBUISetup.h
        TBUIFontInfo.h
        TBUIWindow.cc TBUIWindow.h
    )
    fips_dir(tb)
    fips_files(
        tb_config.h
        tbMgr.cc tbMgr.h
        tbOryolRootWidget.cc tbOryolRootWidget.h
        tbOryolRenderer.cc tbOryolRenderer.h
        tbOryolFile.cc tbOryolFile.h
        tbOryolSystem.cc
        TBUIResourceContainer.cc TBUIResourceContainer.h
    )
    fips_deps(Gfx Time turbobadger)
fips_end_module()
