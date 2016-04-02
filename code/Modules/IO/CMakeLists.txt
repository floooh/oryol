#-------------------------------------------------------------------------------
#   oryol IO module
#-------------------------------------------------------------------------------
fips_begin_module(IO)
    fips_vs_warning_level(3)
    fips_files(
        IO.cc IO.h
    )
    fips_generate(FROM IOProtocol.yml TYPE MessageProtocol)
    fips_dir(Core)
    fips_files(
        ContentType.cc ContentType.h
        IOConfig.h
        IOSetup.h
        IOStatus.cc IOStatus.h
        URL.cc URL.h
        URLBuilder.cc URLBuilder.h
        assignRegistry.cc assignRegistry.h
        schemeRegistry.cc schemeRegistry.h
        loadQueue.cc loadQueue.h
    )
    fips_dir(FS)
    fips_files(
        FileSystem.cc FileSystem.h
        ioLane.cc ioLane.h
        ioRequestRouter.cc ioRequestRouter.h
    )
    fips_deps(Messaging Core)
fips_end_module()

fips_begin_unittest(IO)
    fips_vs_warning_level(3)
    fips_dir(UnitTests)
    fips_files(
        ContentTypeTest.cc
        IOFacadeTest.cc
        IOStatusTest.cc
        URLBuilderTest.cc
        URLTest.cc
        assignRegistryTest.cc
        schemeRegistryTest.cc
    )
    fips_deps(IO Messaging Core)
fips_end_unittest()
