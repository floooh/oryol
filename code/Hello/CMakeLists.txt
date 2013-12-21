#-------------------------------------------------------------------------------
#	hello
#	The most simple hello-world to test the build system.
#-------------------------------------------------------------------------------

oryol_begin_app(hello cmdline)
    oryol_sources(.)
    oryol_deps(Core)
oryol_end_app()



