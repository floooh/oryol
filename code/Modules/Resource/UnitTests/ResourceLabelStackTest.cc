//------------------------------------------------------------------------------
//  ResourceLabelStackTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Resource/Core/ResourceLabelStack.h"

using namespace Oryol;

TEST(ResourceLabelStackTest) {

    ResourceLabelStack stack;
    CHECK(stack.Size() == 0);
    stack.Push(1);
    CHECK(stack.Size() == 1);
    CHECK(stack.Peek() == 1);
    CHECK(1 == stack.Pop());
    CHECK(stack.Size() == 0);
    stack.Push(1);
    CHECK(stack.Size() == 1);
    CHECK(stack.Peek() == 1);
    stack.Push(2);
    CHECK(stack.Size() == 2);
    CHECK(stack.Peek() == 2);
    CHECK(2 == stack.Pop());
    CHECK(stack.Size() == 1);
    stack.Push(3);
    CHECK(stack.Size() == 2);
    CHECK(stack.Peek() == 3);
    stack.Push(4);
    CHECK(stack.Size() == 3);
    CHECK(stack.Peek() == 4);
    CHECK(4 == stack.Pop());
    CHECK(3 == stack.Pop());
    CHECK(1 == stack.Pop());
    CHECK(stack.Size() == 0);
}
