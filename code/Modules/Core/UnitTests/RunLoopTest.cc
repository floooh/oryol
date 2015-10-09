//------------------------------------------------------------------------------
//  ArrayMapTest.cc
//  Test ArrayMap class.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/RunLoop.h"

using namespace Oryol;

TEST(RunLoopTest) {
    RunLoop runLoop;
    int x = 0;
    int y = 0;
    auto id0 = runLoop.Add([&x]() { x++; });
    runLoop.Run();
    CHECK(x == 1);
    CHECK(y == 0);
    auto id1 = runLoop.Add([&y]() { y+=2; });
    runLoop.Run();
    CHECK(x == 2);
    CHECK(y == 2);
    runLoop.Remove(id0);
    runLoop.Run();
    CHECK(x == 2);
    CHECK(y == 4);
    runLoop.Remove(id1);
    runLoop.Run();
    CHECK(x == 2);
    CHECK(y == 4);
}
