//------------------------------------------------------------------------------
//  AsyncQueueTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Messaging/AsyncQueue.h"
#include "Messaging/Dispatcher.h"
#include "Messaging/UnitTests/TestProtocol.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::Messaging;

static int val0 = 0;
static int val1 = 0;
static void MsgHandler1(const Ptr<TestProtocol::TestMsg1>& msg) {
    val0++;
    msg->SetHandled();
}
static void MsgHandler2(const Ptr<TestProtocol::TestMsg2>& msg) {
    val1++;
    msg->SetHandled();
}

TEST(AsyncQueueTest) {
    
    // create an uncapped async queue port and set a dispatcher port as
    // forwarding port which handle messages
    Ptr<AsyncQueue> asyncQueue = AsyncQueue::Create();
    Ptr<Dispatcher<TestProtocol>> dispatcher = Dispatcher<TestProtocol>::Create();
    dispatcher->Subscribe<TestProtocol::TestMsg1>(&MsgHandler1);
    dispatcher->Subscribe<TestProtocol::TestMsg2>(&MsgHandler2);
    asyncQueue->SetForwardingPort(dispatcher);
    
    // send messages, these should be queued
    Ptr<TestProtocol::TestMsg1> msg1_1 = TestProtocol::TestMsg1::Create();
    Ptr<TestProtocol::TestMsg1> msg1_2 = TestProtocol::TestMsg1::Create();
    Ptr<TestProtocol::TestMsg2> msg2_1 = TestProtocol::TestMsg2::Create();
    Ptr<TestProtocol::TestMsg2> msg2_2 = TestProtocol::TestMsg2::Create();
    
    CHECK(asyncQueue->Put(msg1_1));
    CHECK(asyncQueue->Put(msg2_1));
    CHECK(asyncQueue->Put(msg1_2));
    CHECK(asyncQueue->Put(msg2_2));
    CHECK(val0 == 0);
    CHECK(val1 == 0);
    CHECK(asyncQueue->GetNumQueuedMessages() == 4);
    
    // process queued messages
    asyncQueue->ForwardMessages();
    CHECK(asyncQueue->GetNumQueuedMessages() == 0);
    CHECK(val0 == 2);
    CHECK(val1 == 2);
}
