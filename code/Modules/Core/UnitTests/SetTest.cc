//------------------------------------------------------------------------------
//  SetTest.cc
//  Test Set container functionality.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Containers/Set.h"

using namespace Oryol;
using namespace Oryol::Core;

TEST(SetTest) {
    Set<int32> set;
    CHECK(set.GetMinGrow() == ORYOL_CONTAINER_DEFAULT_MIN_GROW);
    CHECK(set.GetMaxGrow() == ORYOL_CONTAINER_DEFAULT_MAX_GROW);
    CHECK(set.Size() == 0);
    CHECK(set.Empty());
    CHECK(set.Capacity() == 0);
    CHECK(!set.Contains(1));
    set.Insert(0);
    set.Insert(3);
    set.Insert(8);
    set.Insert(6);
    set.Insert(4);
    set.Insert(1);
    set.Insert(2);
    set.Insert(7);
    set.Insert(5);
    CHECK(set.Size() == 9);
    CHECK(set.Capacity() == ORYOL_CONTAINER_DEFAULT_MIN_GROW);
    CHECK(!set.Empty());
    CHECK(set.Contains(4));
    CHECK(!set.Contains(11));
    CHECK(nullptr == set.Find(11));
    CHECK(nullptr == set.Find(-1));
    CHECK(*set.Find(0) == 0);
    CHECK(*set.Find(1) == 1);
    CHECK(*set.Find(2) == 2);
    CHECK(*set.Find(3) == 3);
    CHECK(*set.Find(4) == 4);
    CHECK(*set.Find(5) == 5);
    CHECK(*set.Find(6) == 6);
    CHECK(*set.Find(7) == 7);
    CHECK(*set.Find(8) == 8);
    
    // ValueAtIndex
    CHECK(set.ValueAtIndex(0) == 0);
    CHECK(set.ValueAtIndex(1) == 1);
    CHECK(set.ValueAtIndex(2) == 2);
    CHECK(set.ValueAtIndex(3) == 3);
    CHECK(set.ValueAtIndex(4) == 4);
    CHECK(set.ValueAtIndex(5) == 5);
    CHECK(set.ValueAtIndex(6) == 6);
    CHECK(set.ValueAtIndex(7) == 7);
    CHECK(set.ValueAtIndex(8) == 8);
    
    // Erase
    set.Erase(3);
    CHECK(set.Size() == 8);
    CHECK(nullptr == set.Find(3));
    CHECK(*set.Find(0) == 0);
    CHECK(*set.Find(1) == 1);
    CHECK(*set.Find(2) == 2);
    CHECK(*set.Find(4) == 4);
    CHECK(*set.Find(5) == 5);
    CHECK(*set.Find(6) == 6);
    CHECK(*set.Find(7) == 7);
    CHECK(*set.Find(8) == 8);
    
    // test begin/end
    CHECK(set.begin() == &set.ValueAtIndex(0));
    CHECK(set.end() == &set.ValueAtIndex(7) + 1);
    
    // test clear
    set.Clear();
    CHECK(set.Size() == 0);
    CHECK(set.Empty());
}
