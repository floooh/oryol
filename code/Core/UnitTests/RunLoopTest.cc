//------------------------------------------------------------------------------
//  ArrayMapTest.cc
//  Test ArrayMap class.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/RunLoop.h"

using namespace Oryol;
using namespace Oryol::Core;

static int32 cb0Value = 0;

static void callback0() {
    cb0Value++;
}

class callbackClass {
public:
    void callback() {
        value++;
    }
    int32 value{0};
};

TEST(RunLoopTest) {
    Ptr<RunLoop> runLoop = RunLoop::Create();
    CHECK(runLoop->Callbacks().Empty());
    runLoop->Run();
    CHECK(0 == cb0Value);
    
    runLoop->Add(RunLoop::Callback(StringAtom("callback0"), 5, std::function<void()>(&callback0)));
    callbackClass callbackObj;
    CHECK(0 == callbackObj.value);
    runLoop->Add(RunLoop::Callback(StringAtom("callback1"), 0, std::function<void()>(std::bind(&callbackClass::callback, &callbackObj))));
    runLoop->Run();
    CHECK(1 == cb0Value);
    CHECK(1 == callbackObj.value);
    CHECK(runLoop->Callbacks().ValueAtIndex(0).Name() == "callback1");
    CHECK(runLoop->Callbacks().ValueAtIndex(1).Name() == "callback0");
    runLoop->Remove(StringAtom("callback1"));
    CHECK(runLoop->Callbacks().Size() == 2);
    CHECK(runLoop->Callbacks().ValueAtIndex(0).Name() == "callback1");
    CHECK(runLoop->Callbacks().ValueAtIndex(1).Name() == "callback0");
    CHECK(!runLoop->Callbacks().ValueAtIndex(0).IsValid());
    runLoop->Run();
    CHECK(2 == cb0Value);
    CHECK(1 == callbackObj.value);
    CHECK(runLoop->Callbacks().Size() == 1);
    CHECK(runLoop->Callbacks().ValueAtIndex(0).Name() == "callback0");
    runLoop = 0;
}
