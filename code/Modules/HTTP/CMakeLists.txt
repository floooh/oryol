#-------------------------------------------------------------------------------
#   oryol HTTP module
#-------------------------------------------------------------------------------
fips_begin_module(HTTP)
    fips_files(
        HTTPClient.cc HTTPClient.h
        HTTPFileSystem.cc HTTPFileSystem.h
        HTTPMethod.cc HTTPMethod.h
        HTTPProtocol.py
        urlLoader.h
    )
    fips_dir(base)
    fips_files(baseURLLoader.cc baseURLLoader.h)
    if (FIPS_OSX)
        fips_dir(osx)
        fips_files(osxURLLoader.mm osxURLLoader.h)
    endif()
    if (FIPS_LINUX OR FIPS_ANDROID)
        fips_dir(curl)
        fips_files(curlURLLoader.cc curlURLLoader.h)
    endif()
    if (FIPS_WINDOWS)
        fips_dir(windows)
        fips_files(winURLLoader.cc winURLLoader.h)
    endif()
    if (FIPS_EMSCRIPTEN)
        fips_dir(emsc)
        fips_files(emscURLLoader.cc emscURLLoader.h)
    endif()
    if (FIPS_PNACL)
        fips_dir(pnacl)
        fips_files(pnaclURLLoader.cc pnaclURLLoader.h)
    endif()

    fips_deps(IO Messaging Core)
    if (FIPS_WINDOWS)
        fips_deps(WinHttp)
    endif()
    if (FIPS_ANDROID OR FIPS_LINUX)
        fips_deps(curl zlib)
    endif()
    if (FIPS_LINUX)
        fips_deps(rt)
    endif()
fips_end_module()

fips_begin_unittest(HTTP)
    fips_dir(UnitTests)
    fips_files(HTTPClientTest.cc HTTPFileSystemTest.cc HTTPMethodTest.cc)
    fips_deps(IO Messaging HTTP Core)
    fips_frameworks_osx(Foundation)
fips_end_unittest()
