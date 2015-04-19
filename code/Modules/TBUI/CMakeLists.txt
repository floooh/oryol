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
        tbOryolBitmap.cc tbOryolBitmap.h
        tbOryolBatchRenderer.cc tbOryolBatchRenderer.h
        tbOryolFile.cc tbOryolFile.h
        tbOryolSystem.cc
        TBUIResourceContainer.cc TBUIResourceContainer.h
    )
    fips_generate(FROM TBUIShaders.shd TYPE Shader)
    fips_deps(Gfx Input Time turbobadger)
fips_end_module()
