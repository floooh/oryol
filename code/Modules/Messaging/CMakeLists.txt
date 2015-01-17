#-------------------------------------------------------------------------------
#   oryol Messaging module
#-------------------------------------------------------------------------------
fips_begin_module(Messaging)
    fips_files(
        AsyncQueue.cc AsyncQueue.h
        Broadcaster.cc Broadcaster.h
        Dispatcher.h
        Message.cc Message.h
        Port.cc Port.h
        Protocol.h
        Serializer.h
        ThreadedQueue.cc ThreadedQueue.h
        Types.h
    )
    fips_deps(Core)
fips_end_module()

fips_begin_unittest(Messaging)
    fips_dir(UnitTests)
    fips_files(
        AsyncQueueTest.cc
        DispatcherTest.cc
        SerializerTest.cc
        TestProtocol.py
        TestProtocol2.py
        ThreadedQueueTest.cc
    )
    fips_deps(Messaging Core)
fips_end_unittest()
