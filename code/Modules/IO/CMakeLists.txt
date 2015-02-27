#-------------------------------------------------------------------------------
#   oryol IO module
#-------------------------------------------------------------------------------
fips_begin_module(IO)
    fips_files(
        IO.cc IO.h
    )
    fips_generate(FROM IOProtocol.yml TYPE MessageProtocol)
    fips_dir(Core)
    fips_files(
        ContentType.cc ContentType.h
        IOConfig.h
        IOQueue.cc IOQueue.h
        IOSetup.h
        IOStatus.cc IOStatus.h
        IOLoader.cc IOLoader.h
        OpenMode.cc OpenMode.h
        URL.cc URL.h
        URLBuilder.cc URLBuilder.h
        assignRegistry.cc assignRegistry.h
        schemeRegistry.cc schemeRegistry.h
    )
    fips_dir(FS)
    fips_files(
        FileSystem.cc FileSystem.h
        ioLane.cc ioLane.h
        ioRequestRouter.cc ioRequestRouter.h
    )
    fips_dir(Stream)
    fips_files(
        BinaryStreamReader.h
        BinaryStreamWriter.h
        MemoryStream.cc MemoryStream.h
        Stream.cc Stream.h
        StreamReader.cc StreamReader.h
        StreamWriter.cc StreamWriter.h
    ) 
    fips_deps(Messaging Core)
fips_end_module()

fips_begin_unittest(IO)
    fips_dir(UnitTests)
    fips_files(
        BinaryStreamReaderWriterTest.cc
        ContentTypeTest.cc
        IOFacadeTest.cc
        IOStatusTest.cc
        OpenModeTest.cc
        URLBuilderTest.cc
        URLTest.cc
        assignRegistryTest.cc
        schemeRegistryTest.cc
    )
    fips_deps(IO Messaging Core)
fips_end_unittest()
