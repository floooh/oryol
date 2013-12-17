#-------------------------------------------------------------------------------
#	hello
#	The most simple hello-world to test the build system.
#-------------------------------------------------------------------------------

oryol_begin_app(hello cmdline)
    oryol_sources(.)
    oryol_deps(core)
oryol_end_app()



