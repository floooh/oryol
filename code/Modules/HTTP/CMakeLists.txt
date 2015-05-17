#-------------------------------------------------------------------------------
#   oryol HTTP module
#-------------------------------------------------------------------------------
fips_begin_module(HTTP)
    fips_vs_warning_level(3)
    fips_files(
        HTTPClient.cc HTTPClient.h
        HTTPFileSystem.cc HTTPFileSystem.h
        HTTPMethod.cc HTTPMethod.h
        urlLoader.h
    )
    fips_generate(TYPE MessageProtocol FROM HTTPProtocol.yml SOURCE HTTPProtocol.cc HEADER HTTPProtocol.h)
    fips_dir(base)
    fips_files(baseURLLoader.cc baseURLLoader.h)
    if (ORYOL_USE_LIBCURL)
        fips_dir(curl)
        fips_files(curlURLLoader.cc curlURLLoader.h)
    elseif (FIPS_OSX)
        fips_dir(osx)
        fips_files(osxURLLoader.mm osxURLLoader.h)
    elseif (FIPS_WINDOWS)
        fips_dir(windows)
        fips_files(winURLLoader.cc winURLLoader.h)
    elseif (FIPS_EMSCRIPTEN)
        fips_dir(emsc)
        fips_files(emscURLLoader.cc emscURLLoader.h)
    elseif (FIPS_PNACL)
        fips_dir(pnacl)
        fips_files(pnaclURLLoader.cc pnaclURLLoader.h)
    endif()

    fips_deps(IO Messaging Core)
    if (FIPS_WINDOWS)
        fips_libs(WinHttp)
    endif()
    if (FIPS_ANDROID OR FIPS_LINUX OR FIPS_MACOS)
        fips_libs(curl zlib)
    endif()
    if (FIPS_LINUX)
        fips_libs(rt)
    endif()
fips_end_module()

fips_begin_unittest(HTTP)
    fips_vs_warning_level(3)
    fips_dir(UnitTests)
    fips_files(HTTPClientTest.cc HTTPFileSystemTest.cc HTTPMethodTest.cc)
    fips_deps(IO Messaging HTTP Core)
    fips_frameworks_osx(Foundation)
fips_end_unittest()
