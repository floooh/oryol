//------------------------------------------------------------------------------
//  DispatcherTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Messaging/Dispatcher.h"
#include "Messaging/Broadcaster.h"
#include "TestProtocol.h"
#include "TestProtocol2.h"

using namespace Oryol;

// a global message handler function
static int32 val = 0;
static void GlobalHandler(const Ptr<TestProtocol::TestMsg1>& msg) {
    CHECK(msg->MessageId() == TestProtocol::MessageId::TestMsg1Id);
    CHECK(msg->GetInt8Val() == 8);
    CHECK(msg->GetInt16Val() == 16);
    CHECK(msg->GetInt32Val() == 32);
    CHECK(msg->GetInt64Val() == 64);
    val++;
}

// handler functions inside an object
class HandlerClass {
public:
    void Handle(const Ptr<TestProtocol::TestMsg2>& msg) {
        CHECK(msg->MessageId() == TestProtocol::MessageId::TestMsg2Id);
        CHECK(msg->GetUInt16Val() == 16);
        CHECK(msg->GetStringVal() == "BLA");
        this->val++;
    };
    
    int32 val = 0;
};

TEST(DispatcherTest) {
    
    // build a broad caster with 2 dispatchers
    Ptr<Broadcaster> sink = Broadcaster::Create();
    Ptr<Dispatcher<TestProtocol>> disp0 = Dispatcher<TestProtocol>::Create();
    Ptr<Dispatcher<TestProtocol2>> disp1 = Dispatcher<TestProtocol2>::Create();
    
    // define a global callback method
    disp0->Subscribe<TestProtocol::TestMsg1>(&GlobalHandler);

    // define an object method as callback function
    using namespace std::placeholders;
    HandlerClass handlerObj;
    disp0->Subscribe<TestProtocol::TestMsg2>(std::bind(&HandlerClass::Handle, &handlerObj, _1));
    
    // add dispatchers
    sink->Subscribe(disp0);
    sink->Subscribe(disp1);
    
    // send a few messages
    Ptr<TestProtocol::TestMsg1> msg0 = TestProtocol::TestMsg1::Create();
    msg0->SetInt8Val(8);
    msg0->SetInt16Val(16);
    msg0->SetInt32Val(32);
    msg0->SetInt64Val(64);
    sink->Put(msg0);
    CHECK(val == 1);
    
    Ptr<TestProtocol::TestMsg2> msg1 = TestProtocol::TestMsg2::Create();
    msg1->SetUInt16Val(16);
    msg1->SetStringVal("BLA");
    sink->Put(msg1);
    CHECK(handlerObj.val == 1);

    sink = 0;
    disp0 = 0;
    disp1 = 0;
}


