//------------------------------------------------------------------------------
//  ThreadedQueueTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Messaging/ThreadedQueue.h"
#include "Messaging/Dispatcher.h"
#include "Messaging/UnitTests/TestProtocol.h"
#include <chrono>
#include <thread>

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::Messaging;
using namespace std::chrono;

// message handling functions (these run in threads)
static int value0;
static void HandleTestMsg1(const Ptr<TestProtocol::TestMsg1>& msg) {
//    Log::Info("TestMsg1 received!\n");
    value0++;
    msg->SetHandled();
}
static int value1;
static void HandleTestMsg2(const Ptr<TestProtocol::TestMsg2>& msg) {
//    Log::Info("TestMsg2 received!\n");
    value1++;
    msg->SetHandled();
}

TEST(ThreadedQueueTest) {

    // create a Dispatcher port, this will run in the worker thread!
    Ptr<Dispatcher<TestProtocol>> disp = Dispatcher<TestProtocol>::Create();
    disp->Subscribe<TestProtocol::TestMsg1>(&HandleTestMsg1);
    disp->Subscribe<TestProtocol::TestMsg2>(&HandleTestMsg2);
    
    // create a ThreadedQueue port and attach dispatcher to it
    Ptr<ThreadedQueue> threadedQueue = ThreadedQueue::Create(disp);
    threadedQueue->StartThread();
    
    // send messages, these should be queued up
    Ptr<TestProtocol::TestMsg1> msg1 = TestProtocol::TestMsg1::Create();
    threadedQueue->Put(msg1);
    Ptr<TestProtocol::TestMsg2> msg2 = TestProtocol::TestMsg2::Create();
    threadedQueue->Put(msg2);
    
    // do a busy loop...
    while (!(msg1->Handled() && msg2->Handled())) {
        threadedQueue->DoWork();
        std::this_thread::sleep_for(milliseconds(100));
    }
    CHECK(value0 == 1);
    CHECK(value1 == 1);
    
    // now let's create some pressure on the thread...
    time_point<system_clock> start, end;
    start = system_clock::now();
    value0 = 0;
    for (int i = 0; i < 1000; i++) {
        Ptr<TestProtocol::TestMsg1> msg;
        for (int j = 0; j < 1000; j++) {
            msg = TestProtocol::TestMsg1::Create();
            threadedQueue->Put(msg);
        }
        // busy-loop until the last message has been handled
        while (!msg->Handled()) {
            threadedQueue->DoWork();
            std::this_thread::yield();
        }
    }
    CHECK(value0 == 1000000);
    end = system_clock::now();
    duration<double> dur = end - start;
    Log::Info("ThreadedQueue: 1000000 msgs created and handled: %f sec\n", dur.count());
    
    // shutdown the threaded queue
    threadedQueue->StopThread();
    threadedQueue = 0;
}

