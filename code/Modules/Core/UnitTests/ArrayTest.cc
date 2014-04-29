//------------------------------------------------------------------------------
//  ArrayTest.cc
//  Test Array class.
//------------------------------------------------------------------------------
#include "Pre.h"
#include <algorithm>
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Containers/Array.h"
#include "Core/String/String.h"

using namespace Oryol;
using namespace Oryol::Core;

TEST(ArrayTest) {
    
    // create empty array
    Array<int> array0;
    CHECK(array0.GetMinGrow() == ORYOL_CONTAINER_DEFAULT_MIN_GROW);
    CHECK(array0.GetMaxGrow() == ORYOL_CONTAINER_DEFAULT_MAX_GROW);
    CHECK(array0.Size() == 0);
    CHECK(array0.Capacity() == 0);
    CHECK(array0.Empty());
    CHECK(array0.Spare() == 0);
    
    // copy-construct from empty array
    Array<int> array1(array0);
    CHECK(array1.GetMinGrow() == ORYOL_CONTAINER_DEFAULT_MIN_GROW);
    CHECK(array1.GetMaxGrow() == ORYOL_CONTAINER_DEFAULT_MAX_GROW);
    CHECK(array1.Size() == 0);
    CHECK(array1.Capacity() == 0);
    CHECK(array1.Empty());
    CHECK(array1.Spare() == 0);
    
    // copy-assign empty array
    Array<int> array2 = array0;
    CHECK(array1.GetMinGrow() == ORYOL_CONTAINER_DEFAULT_MIN_GROW);
    CHECK(array1.GetMaxGrow() == ORYOL_CONTAINER_DEFAULT_MAX_GROW);
    CHECK(array1.Size() == 0);
    CHECK(array1.Capacity() == 0);
    CHECK(array1.Empty());
    CHECK(array1.Spare() == 0);
    
    // add some elements
    const int one = 1;
    array0.AddBack(0);
    array0.AddBack(one);
    array0.AddBack(2);
    CHECK(array0.Size() == 3);
    CHECK(array0.Capacity() == ORYOL_CONTAINER_DEFAULT_MIN_GROW);
    CHECK(!array0.Empty());
    CHECK(array0.Spare() == (ORYOL_CONTAINER_DEFAULT_MIN_GROW - 3));
    CHECK(array0[0] == 0);
    CHECK(array0[1] == 1);
    CHECK(array0[2] == 2);
    CHECK(array0.Front() == 0);
    CHECK(array0.Back() == 2);
    
    // copy-construct from non-empty array
    Array<int> array3(array0);
    CHECK(array3.Size() == 3);
    CHECK(array3.Capacity() == 3);
    CHECK(!array3.Empty());
    CHECK(array3.Spare() == 0);
    CHECK(array3[0] == 0);
    CHECK(array3[1] == 1);
    CHECK(array3[2] == 2);
    CHECK(array3.Front() == 0);
    CHECK(array3.Back() == 2);

    // copy-assign from non-empty array
    array2 = array0;
    CHECK(array2.Size() == 3);
    CHECK(array2.Capacity() == 3);
    CHECK(!array2.Empty());
    CHECK(array2.Spare() == 0);
    CHECK(array2[0] == 0);
    CHECK(array2[1] == 1);
    CHECK(array2[2] == 2);
    CHECK(array2.Front() == 0);
    CHECK(array2.Back() == 2);
    
    // move-assign
    array2 = std::move(array0);
    CHECK(array0.Size() == 0);
    CHECK(array2.Size() == 3);
    CHECK(array2.Capacity() == ORYOL_CONTAINER_DEFAULT_MIN_GROW);
    CHECK(!array2.Empty());
    CHECK(array2.Spare() == (ORYOL_CONTAINER_DEFAULT_MIN_GROW - 3));
    CHECK(array2[0] == 0);
    CHECK(array2[1] == 1);
    CHECK(array2[2] == 2);
    CHECK(array2.Front() == 0);
    CHECK(array2.Back() == 2);
    
    // test emplace-back
    array2.EmplaceBack(3);
    CHECK(array2.Size() == 4);
    CHECK(array2[0] == 0);
    CHECK(array2[1] == 1);
    CHECK(array2[2] == 2);
    CHECK(array2[3] == 3);
    
    // check element copy-assignment and move-assignment
    array2[0] = 2;
    CHECK(array2[0] == 2);
    array2[0] = 0;
    
    // check Reserve
    array2.Reserve(128);
    CHECK(array2.Size() == 4);
    CHECK(array2.Capacity() == 132);
    CHECK(array2[0] == 0);
    CHECK(array2[1] == 1);
    CHECK(array2[2] == 2);
    CHECK(array2[3] == 3);
    
    // insert element
    array2.Insert(0, 5);
    CHECK(array2.Size() == 5);
    CHECK(array2[0] == 5);
    CHECK(array2[1] == 0);
    CHECK(array2[2] == 1);
    CHECK(array2[3] == 2);
    CHECK(array2[4] == 3);
    // append with insert (this is valid)
    array2.Insert(5, 6);
    CHECK(array2.Size() == 6);
    CHECK(array2[0] == 5);
    CHECK(array2[1] == 0);
    CHECK(array2[2] == 1);
    CHECK(array2[3] == 2);
    CHECK(array2[4] == 3);
    CHECK(array2[5] == 6);
    array2.Insert(2, 7);
    CHECK(array2.Size() == 7);
    CHECK(array2[0] == 5);
    CHECK(array2[1] == 0);
    CHECK(array2[2] == 7);
    CHECK(array2[3] == 1);
    CHECK(array2[4] == 2);
    CHECK(array2[5] == 3);
    CHECK(array2[6] == 6);
    array2.Insert(4, 8);
    CHECK(array2.Size() == 8);
    CHECK(array2[0] == 5);
    CHECK(array2[1] == 0);
    CHECK(array2[2] == 7);
    CHECK(array2[3] == 1);
    CHECK(array2[4] == 8);
    CHECK(array2[5] == 2);
    CHECK(array2[6] == 3);
    CHECK(array2[7] == 6);
    
    // erase, keep order
    array2.Erase(0);
    CHECK(array2.Size() == 7);
    CHECK(array2[0] == 0);
    CHECK(array2[1] == 7);
    CHECK(array2[2] == 1);
    CHECK(array2[3] == 8);
    CHECK(array2[4] == 2);
    CHECK(array2[5] == 3);
    CHECK(array2[6] == 6);
    array2.Erase(6);
    CHECK(array2.Size() == 6);
    CHECK(array2[0] == 0);
    CHECK(array2[1] == 7);
    CHECK(array2[2] == 1);
    CHECK(array2[3] == 8);
    CHECK(array2[4] == 2);
    CHECK(array2[5] == 3);
    array2.Erase(1);
    CHECK(array2.Size() == 5);
    CHECK(array2[0] == 0);
    CHECK(array2[1] == 1);
    CHECK(array2[2] == 8);
    CHECK(array2[3] == 2);
    CHECK(array2[4] == 3);
    
    // check erase swap
    array2.Trim();
    CHECK(array2.Size() == 5);
    CHECK(array2.Capacity() == 5);
    CHECK(array2[0] == 0);
    CHECK(array2[1] == 1);
    CHECK(array2[2] == 8);
    CHECK(array2[3] == 2);
    CHECK(array2[4] == 3);    
    array2.AddBack(4);
    array2.AddBack(5);
    CHECK(array2.Size() == 7);
    CHECK(array2.Capacity() == (5 + ORYOL_CONTAINER_DEFAULT_MIN_GROW));
    array2.Trim();
    CHECK(array2.Size() == 7);
    CHECK(array2.Capacity() == 7);
    CHECK(array2[0] == 0);
    CHECK(array2[1] == 1);
    CHECK(array2[2] == 8);
    CHECK(array2[3] == 2);
    CHECK(array2[4] == 3);
    CHECK(array2[5] == 4);
    CHECK(array2[6] == 5);
    array2.EraseSwap(2);
    CHECK(array2.Size() == 6);
    CHECK(array2[0] == 1);
    CHECK(array2[1] == 0);
    CHECK(array2[2] == 2);
    CHECK(array2[3] == 3);
    CHECK(array2[4] == 4);
    CHECK(array2[5] == 5);
    array2.EraseSwap(3);
    CHECK(array2.Size() == 5);
    CHECK(array2[0] == 1);
    CHECK(array2[1] == 0);
    CHECK(array2[2] == 2);
    CHECK(array2[3] == 5);
    CHECK(array2[4] == 4);
    array2.EraseSwapBack(1);
    CHECK(array2.Size() == 4);
    CHECK(array2[0] == 1);
    CHECK(array2[1] == 4);
    CHECK(array2[2] == 2);
    CHECK(array2[3] == 5);
    array2.EraseSwapFront(2);
    CHECK(array2.Size() == 3);
    CHECK(array2[0] == 4);
    CHECK(array2[1] == 1);
    CHECK(array2[2] == 5);
    
    array2.Clear();
    CHECK(array2.Size() == 0);
    CHECK(array2.Capacity() == 7);
    CHECK(array2.begin() == array2.end());
    for (int i = 0; i < 16; i++) {
        array2.AddBack(i);
    }
    for (int* p = array2.begin(), i = 0; p != array2.end(); p++, i++) {
        CHECK(*p == i);
    }
    array2.Clear();
    for (int* p = array2.begin(); p != array2.end(); p++) {
        CHECK(false);
    }
    
    // check for for(:) works
    for (int i = 0; i < 16; i++) {
        array2.AddBack(i);
    }
    for (int x : array2) {
        CHECK(array2[x] == x);
    }

    // test some algorithms stuff
    array2.Clear();
    array2.AddBack(1);
    array2.AddBack(3);
    array2.AddBack(5);
    array2.AddBack(7);
    array2.AddBack(9);
    array2.AddBack(11);
    bool allOdd = std::all_of(array2.begin(), array2.end(), [](int i){ return i%2 == 1; });
    CHECK(allOdd);
    bool anyEven = std::any_of(array2.begin(), array2.end(), [](int i){ return i%2 == 0; });
    CHECK(!anyEven);
    array2[2] = 2;
    allOdd = std::all_of(array2.begin(), array2.end(), [](int i){ return i%2 == 1; });
    CHECK(!allOdd);
    anyEven = std::any_of(array2.begin(), array2.end(), [](int i){ return i%2 == 0; });
    CHECK(anyEven);
    std::reverse(array2.begin(), array2.end());
    CHECK(array2[0] == 11);
    CHECK(array2[1] == 9);
    CHECK(array2[2] == 7);
    CHECK(array2[3] == 2);
    CHECK(array2[4] == 3);
    CHECK(array2[5] == 1);
    std::sort(array2.begin(), array2.end());
    CHECK(array2[0] == 1);
    CHECK(array2[1] == 2);
    CHECK(array2[2] == 3);
    CHECK(array2[3] == 7);
    CHECK(array2[4] == 9);
    CHECK(array2[5] == 11);
    
    // FindIndexLinear
    CHECK(array2.FindIndexLinear(3) == 2);
    CHECK(array2.FindIndexLinear(7, 0, 3) == InvalidIndex);
    CHECK(array2.FindIndexLinear(7, 3, InvalidIndex) == 3);
    CHECK(array2.FindIndexLinear(11) == 5);
    
    // construct from initializer list
    Array<String> array4( { "Bla", "Blub", "Blob", "Blubber" } );
    CHECK(array4.Size() == 4);
    CHECK(array4[0] == "Bla");
    CHECK(array4[1] == "Blub");
    CHECK(array4[2] == "Blob");
    CHECK(array4[3] == "Blubber");
}

