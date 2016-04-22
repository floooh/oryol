//------------------------------------------------------------------------------
//  DispatcherTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Messaging/Dispatcher.h"
#include "Messaging/UnitTests/TestProtocol.h"
#include "Messaging/UnitTests/TestProtocol2.h"

using namespace Oryol;

// a global message handler function
static int val = 0;
static void GlobalHandler(const Ptr<TestProtocol::TestMsg1>& msg) {
    CHECK(msg->MessageId() == TestProtocol::MessageId::TestMsg1Id);
    CHECK(msg->Int8Val == 8);
    CHECK(msg->Int16Val == 16);
    CHECK(msg->Int32Val == 32);
    CHECK(msg->Int64Val == 64);
    val++;
}

// handler functions inside an object
class HandlerClass {
public:
    void Handle(const Ptr<TestProtocol::TestMsg2>& msg) {
        CHECK(msg->MessageId() == TestProtocol::MessageId::TestMsg2Id);
        CHECK(msg->UInt16Val == 16);
        CHECK(msg->StringVal == "BLA");
        this->val++;
    };
    
    int val = 0;
};

TEST(DispatcherTest) {
    
    Ptr<Dispatcher<TestProtocol>> disp0 = Dispatcher<TestProtocol>::Create();
    Ptr<Dispatcher<TestProtocol2>> disp1 = Dispatcher<TestProtocol2>::Create();
    
    // define a global callback method
    disp0->Subscribe<TestProtocol::TestMsg1>(&GlobalHandler);

    // define an object method as callback function
    using namespace std::placeholders;
    HandlerClass handlerObj;
    disp0->Subscribe<TestProtocol::TestMsg2>(std::bind(&HandlerClass::Handle, &handlerObj, _1));
    
    // send a few messages
    Ptr<TestProtocol::TestMsg1> msg0 = TestProtocol::TestMsg1::Create();
    msg0->Int8Val = 8;
    msg0->Int16Val = 16;
    msg0->Int32Val = 32;
    msg0->Int64Val = 64;
    disp0->Put(msg0);
    disp1->Put(msg0);
    CHECK(val == 1);
    
    Ptr<TestProtocol::TestMsg2> msg1 = TestProtocol::TestMsg2::Create();
    msg1->UInt16Val = 16;
    msg1->StringVal = "BLA";
    disp0->Put(msg1);
    disp1->Put(msg1);
    CHECK(handlerObj.val == 1);

    disp0 = 0;
    disp1 = 0;
}


