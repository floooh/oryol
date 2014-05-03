#-------------------------------------------------------------------------------
#	hello
#	The most simple hello-world to test the build system.
#-------------------------------------------------------------------------------

if (NOT ORYOL_ANDROID AND NOT ORYOL_IOS AND NOT ORYOL_PNACL)
oryol_begin_app(hello cmdline)
    oryol_sources(.)
    oryol_deps(Core)
oryol_end_app()
endif()


