#-------------------------------------------------------------------------------
#   oryol HTTP module
#-------------------------------------------------------------------------------
oryol_begin_module(HTTP)
oryol_sources(. base)
oryol_sources_osx(osx)
oryol_sources_linux(curl)
oryol_sources_windows(windows)
oryol_sources_emscripten(emsc)
oryol_sources_pnacl(pnacl)
oryol_deps(IO Messaging Core)
if (ORYOL_WINDOWS)
    oryol_deps(WinHttp)
elseif (ORYOL_LINUX)
    oryol_deps(curl zlib)
    oryol_deps(rt)
endif()
oryol_end_module()

oryol_begin_unittest(HTTP)
oryol_sources(UnitTests)
oryol_deps(IO Messaging HTTP Core)
oryol_frameworks_osx(Foundation)
oryol_end_unittest()
