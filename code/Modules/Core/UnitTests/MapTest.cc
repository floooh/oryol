//------------------------------------------------------------------------------
//  MapTest.cc
//  Test Dictionary functionality
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Containers/Map.h"
#include "Core/String/String.h"

using namespace Oryol;

TEST(MapTest) {

    // test simple insertion of unique elements
    Map<int, int> map;
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
    
    // copy construct
    Map<int, int> map1(map);
    CHECK(map1.Size() == 9);
    CHECK(map1.Capacity() == 9); // copy trims
    CHECK(!map1.Empty());
    CHECK(map1.Contains(4));
    CHECK(!map1.Contains(11));
    for (int i = 0; i < 9; i++) {
        CHECK(map1[i] == i);
    }
    
    // copy-assign
    Map<int, int> map2;
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
    
    // index accessor functions
    for (int i = 0; i < 9; i++) {
        CHECK(map.KeyAtIndex(i) == i);
        CHECK(map.ValueAtIndex(i) == i);
    }
    
    // modify values
    map[5] = 6;
    CHECK(map[5] == 6);
    map.ValueAtIndex(7) = 6;
    CHECK(map[7] == 6);
    
    // check FindDuplicate
    CHECK(InvalidIndex == map.FindDuplicate(0));
    map.Add(5, 10);
    map.Add(5, 11);
    CHECK(5 == map.FindDuplicate(0));
    // test erase (single and duplicates)
    CHECK(map.Contains(5));
    CHECK(map.Size() == 11);
    map.Erase(5);   // this should remove all duplicates
    CHECK(!map.Contains(5));
    CHECK(map.Size() == 8);
    CHECK(InvalidIndex == map.FindDuplicate(0));
    
    // erase at front and back
    map.Erase(0);
    CHECK(map.Size() == 7);
    CHECK(!map.Contains(0));
    map.Erase(8);
    CHECK(map.Size() == 6);
    CHECK(!map.Contains(8));
    
    // test bulk add
    Map<int, int> map4;
    map4.BeginBulk();
    for (int i = 31; i >= 0; i--) {
        KeyValuePair<int, int> kvp(i, i);
        map4.AddBulk(kvp);
    }
    map4.EndBulk();
    for (int i = 0; i < 32; i++) {
        CHECK(map4.KeyAtIndex(i) == i);
        CHECK(map4.ValueAtIndex(i) == i);
        CHECK(map4[i] == i);
    }
    
    // FindIndex
    CHECK(map4.FindIndex(0) == 0);
    CHECK(map4.FindIndex(31) == 31);
    CHECK(map4.FindIndex(16) == 16);
    CHECK(map4.FindIndex(100) == InvalidIndex);
    
    // test InsertUnique
    CHECK(map4.AddUnique(32, 32));
    CHECK(!map4.AddUnique(32, 33));
    CHECK(map4[32] == 32);
    int index = map4.FindIndex(32);
    CHECK(InvalidIndex == map4.FindDuplicate(index));

    // test for https://github.com/floooh/oryol/issues/271
    Map<int32_t, String> testMap;
    for (int32_t counter = 0; counter < 20; ++counter) {
        testMap.Add(counter, "testString");
        testMap.Erase(counter);
    }
    
    // initializer_list
    Map<String, String> map_il = {
        {"Wittiness Level", "Not very high"},
        {"Test Effectiveness", "Pretty Meh"}
    };
    CHECK(map_il["Wittiness Level"] == "Not very high");
    CHECK(map_il["Test Effectiveness"] == "Pretty Meh");
}
