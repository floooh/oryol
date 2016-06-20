//------------------------------------------------------------------------------
//  ArrayMapTest.cc
//  Test ArrayMap class.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Containers/ArrayMap.h"

using namespace Oryol;

TEST(ArrayMapTest) {
    // test simple insertion of unique elements
    ArrayMap<int, int> map;
    CHECK(map.GetMinGrow() == ORYOL_CONTAINER_DEFAULT_MIN_GROW);
    CHECK(map.GetMaxGrow() == ORYOL_CONTAINER_DEFAULT_MAX_GROW);
    CHECK(map.Size() == 0);
    CHECK(map.Empty());
    CHECK(map.Capacity() == 0);
    CHECK(!map.Contains(1));
    map.Add(0, 0);
    map.Add(3, 3);
    map.Add(8, 8);
    map.Add(6, 6);
    map.Add(4, 4);
    map.Add(1, 1);
    map.Add(2, 2);
    map.Add(7, 7);
    map.Add(5, 5);
    CHECK(map.Size() == 9);
    CHECK(map.Capacity() == ORYOL_CONTAINER_DEFAULT_MIN_GROW);
    CHECK(!map.Empty());
    CHECK(map.Contains(4));
    CHECK(!map.Contains(11));
    for (int i = 0; i < 9; i++) {
        CHECK(map[i] == i);
    }
    
    // ValueAtIndex
    CHECK(map.ValueAtIndex(0) == 0);
    CHECK(map.ValueAtIndex(1) == 3);
    CHECK(map.ValueAtIndex(2) == 8);
    CHECK(map.ValueAtIndex(3) == 6);
    CHECK(map.ValueAtIndex(4) == 4);
    CHECK(map.ValueAtIndex(5) == 1);
    CHECK(map.ValueAtIndex(6) == 2);
    CHECK(map.ValueAtIndex(7) == 7);
    CHECK(map.ValueAtIndex(8) == 5);
    
    // FindValueIndex
    CHECK(map.FindValueIndex(0) == 0);
    CHECK(map.FindValueIndex(3) == 1);
    CHECK(map.FindValueIndex(8) == 2);
    CHECK(map.FindValueIndex(6) == 3);
    CHECK(map.FindValueIndex(4) == 4);
    CHECK(map.FindValueIndex(1) == 5);
    CHECK(map.FindValueIndex(2) == 6);
    CHECK(map.FindValueIndex(7) == 7);
    CHECK(map.FindValueIndex(5) == 8);
    
    // copy construct
    ArrayMap<int, int> map1(map);
    CHECK(map1.Size() == 9);
    CHECK(map1.Capacity() == 9); // copy trims
    CHECK(!map1.Empty());
    CHECK(map1.Contains(4));
    CHECK(!map1.Contains(11));
    for (int i = 0; i < 9; i++) {
        CHECK(map1[i] == i);
    }
    
    // copy-assign
    ArrayMap<int, int> map2;
    map2 = map;
    CHECK(map2.Size() == 9);
    CHECK(map2.Capacity() == 9);    // copy trims
    CHECK(!map2.Empty());
    CHECK(map2.Contains(4));
    CHECK(!map2.Contains(11));
    for (int i = 0; i < 9; i++) {
        CHECK(map2[i] == i);
    }
    
    // trim and clear
    map.Trim();
    CHECK(map.Size() == 9);
    CHECK(map.Capacity() == 9);
    CHECK(!map.Empty());
    CHECK(map.Contains(4));
    CHECK(!map.Contains(11));
    for (int i = 0; i < 9; i++) {
        CHECK(map[i] == i);
    }
    map.Clear();
    CHECK(map.Size() == 0);
    CHECK(map.Capacity() == 9);
    CHECK(map.Empty());
    CHECK(!map.Contains(2));
    
    // move-assign
    map = std::move(map2);
    CHECK(map2.Size() == 0);
    CHECK(map2.Capacity() == 0);
    CHECK(map.Size() == 9);
    CHECK(map.Capacity() == 9);    // copy trims
    CHECK(!map.Empty());
    CHECK(map.Contains(4));
    CHECK(!map.Contains(11));
    for (int i = 0; i < 9; i++) {
        CHECK(map[i] == i);
    }
    
    // modify values
    map[5] = 6;
    CHECK(map[5] == 6);
    
    // erase
    map.Erase(5);
    CHECK(map.Size() == 8);
    CHECK(!map.Contains(5));
    CHECK(map[0] == 0);
    CHECK(map[1] == 1);
    CHECK(map[2] == 2);
    CHECK(map[3] == 3);
    CHECK(map[4] == 4);
    CHECK(map[6] == 6);
    CHECK(map[7] == 7);
    CHECK(map[8] == 8);
    CHECK(map.ValueAtIndex(0) == 0);
    CHECK(map.ValueAtIndex(1) == 3);
    CHECK(map.ValueAtIndex(2) == 8);
    CHECK(map.ValueAtIndex(3) == 6);
    CHECK(map.ValueAtIndex(4) == 4);
    CHECK(map.ValueAtIndex(5) == 1);
    CHECK(map.ValueAtIndex(6) == 2);
    CHECK(map.ValueAtIndex(7) == 7);
    
    // erase-swap
    map.EraseSwap(1);
    CHECK(map.Size() == 7);
    CHECK(!map.Contains(1));
    CHECK(map[0] == 0);
    CHECK(map[2] == 2);
    CHECK(map[3] == 3);
    CHECK(map[4] == 4);
    CHECK(map[6] == 6);
    CHECK(map[7] == 7);
    CHECK(map[8] == 8);
    CHECK(map.ValueAtIndex(0) == 0);
    CHECK(map.ValueAtIndex(1) == 3);
    CHECK(map.ValueAtIndex(2) == 8);
    CHECK(map.ValueAtIndex(3) == 6);
    CHECK(map.ValueAtIndex(4) == 4);
    CHECK(map.ValueAtIndex(5) == 7);
    CHECK(map.ValueAtIndex(6) == 2);
}


