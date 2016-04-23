fips_begin_module(TBUI)
    fips_vs_warning_level(3)
    fips_vs_disable_warnings(4267)   # converting from 64-bit to 32-bit values, this happens often in Turbobadger source
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
        tbResourceContainer.cc tbResourceContainer.h
    )
    oryol_shader(TBUIShaders.shd)
    fips_deps(Gfx Input turbobadger)
fips_end_module()
