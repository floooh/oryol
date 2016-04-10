#-------------------------------------------------------------------------------
#   oryol Messaging module
#-------------------------------------------------------------------------------
fips_begin_module(Messaging)
    fips_vs_warning_level(3)
    fips_files(
        Dispatcher.h
        Message.cc Message.h
        Port.cc Port.h
        Protocol.h
        ThreadedQueue.cc ThreadedQueue.h
        Types.h
    )
    fips_deps(Core)
fips_end_module()

fips_begin_unittest(Messaging)
    fips_vs_warning_level(3)
    fips_dir(UnitTests)
    fips_files(
        DispatcherTest.cc
        ThreadedQueueTest.cc
    )
    fips_generate(FROM TestProtocol.yml TYPE MessageProtocol SOURCE TestProtocol.cc HEADER TestProtocol.h)
    fips_generate(TYPE MessageProtocol FROM TestProtocol2.yml)
    fips_deps(Messaging Core)
fips_end_unittest()
