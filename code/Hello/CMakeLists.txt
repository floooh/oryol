#-------------------------------------------------------------------------------
#	hello
#	The most simple hello-world to test the build system.
#-------------------------------------------------------------------------------

if (NOT FIPS_ANDROID AND NOT FIPS_IOS AND NOT FIPS_PNACL)
fips_begin_app(hello cmdline)
    fips_sources(.)
    fips_deps(Core)
fips_end_app()
endif()


