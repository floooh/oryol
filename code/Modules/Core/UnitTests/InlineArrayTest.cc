//------------------------------------------------------------------------------
//  InlineArrayTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include <algorithm>
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Containers/InlineArray.h"
#include "Core/String/String.h"
#include "Core/Containers/KeyValuePair.h"

using namespace Oryol;

struct myClass {
    myClass() { };
    myClass(int v): val(v) { };
    myClass(int v, int w): val(v), wal(w) { };
    ~myClass() { val = 0; wal = 0; };
    myClass(const myClass& rhs) { val = rhs.val; wal = rhs.wal; };
    myClass(myClass&& rhs) { val = rhs.val; wal = rhs.wal; rhs.val = 0; rhs.wal = 0; };
    void operator=(const myClass& rhs) { val = rhs.val; wal = rhs.wal; };
    void operator=(myClass&& rhs) { val = rhs.val; wal = rhs.wal; rhs.val = 0; rhs.wal = 0; };

    int val = 1;
    int wal = 2;
};

TEST(InlineArrayTest) {

    InlineArray<myClass, 32> arr0;
    CHECK(arr0.Empty());
    CHECK(arr0.Size() == 0);
    CHECK(arr0.Capacity() == 32);
    CHECK(arr0.Spare() == 32);

    // populate array 
    myClass one(1), two(2), three(3);
    CHECK(arr0.Add(one).val == 1);
    CHECK(!arr0.Empty());
    CHECK(arr0.Size() == 1);
    CHECK(arr0.Spare() == 31);
    CHECK(arr0[0].val == 1);
    CHECK(arr0.Add(std::move(two)).val == 2);
    CHECK(two.val == 0);
    CHECK(arr0.Size() == 2);
    CHECK(arr0.Spare() == 30);
    CHECK(arr0[1].val == 2);
    two = myClass(2);
    arr0.Add({ one, two, three });
    CHECK(arr0.Size() == 5);
    CHECK(arr0[0].val == 1);
    CHECK(arr0[1].val == 2);
    CHECK(arr0[2].val == 1);
    CHECK(arr0[3].val == 2);
    CHECK(arr0[4].val == 3);

    // iterate over array
    int values[] = { 1, 2, 1, 2, 3 };
    int i = 0;
    for (auto& v : arr0) {
        CHECK(v.val == values[i++]);
    }

    // copy construction
    InlineArray<myClass, 32> arr1(arr0);
    CHECK(arr1.Size() == arr0.Size());
    CHECK(arr1.Capacity() == arr0.Capacity());
    for (int i = 0; i < arr0.Size(); i++) {
        CHECK(arr0[i].val == arr1[i].val);
    }

    // move construction
    InlineArray<myClass, 32> arr2(std::move(arr0));
    CHECK(arr2.Size() == 5);
    CHECK(arr0.Size() == 0);
    // EVIL CODE to test whether moved elements have been default-initialized,
    // don't do this in real code
    CHECK(arr0.begin()->val == 0);
    // check if movement was successful
    for (int i = 0; i < arr2.Size(); i++) {
        CHECK(arr2[i].val == arr1[i].val);
    }

    // copy-assignment
    arr0 = { 5, 6, 7, 8, 9, 10 };
    CHECK(arr0.Size() == 6);
    arr1 = arr0;
    CHECK(arr0.Size() == 6);
    CHECK(arr1.Size() == 6);
    for (int i = 0; i < 6; i++) {
        CHECK(arr0[i].val == arr1[i].val);
    }
    // move-assignment 
    arr2 = std::move(arr0);
    CHECK(arr0.Empty());
    CHECK(arr2.Size() == 6);
    for (int i = 0; i < 6; i++) {
        CHECK(arr2[i].val == arr1[i].val);
    }

    // check if copying and moving a smaller array into a bigger
    // array works (the left-over items in target array must 
    // be default-initialized afterwards)
    arr0 = { 1, 2, 3 };
    arr1 = { 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 };
    arr2 = arr1;
    arr1 = arr0;
    CHECK(arr1[0].val == 1);
    CHECK(arr1[1].val == 2);
    CHECK(arr1[2].val == 3);
    // EVIL, DON'T DO THIS in actual code
    for (int i = 3; i < 10; i++) {
        CHECK(arr1.begin()[i].val == 1);
    }

    arr2 = std::move(arr0);
    CHECK(arr0.Empty());
    CHECK(arr2[0].val == 1);
    CHECK(arr2[1].val == 2);
    CHECK(arr2[2].val == 3);
    // EVIL, DON'T DO THIS in actual code
    for (int i = 3; i < 10; i++) {
        CHECK(arr2.begin()[i].val == 1);
    }

    // insertion
    arr0 = { 1, 2, 3 };
    arr0.Insert(3, 4);  // insert at end
    CHECK(arr0.Size() == 4);
    CHECK(arr0[0].val == 1);
    CHECK(arr0[1].val == 2);
    CHECK(arr0[2].val == 3);
    CHECK(arr0[3].val == 4);
    arr0.Insert(0, 0);  // insert at beginning
    CHECK(arr0.Size() == 5);
    CHECK(arr0[0].val == 0);
    CHECK(arr0[1].val == 1);
    CHECK(arr0[2].val == 2);
    CHECK(arr0[3].val == 3);
    CHECK(arr0[4].val == 4);
    arr0.Insert(2, 23); // insert in the middle
    CHECK(arr0.Size() == 6);
    CHECK(arr0[0].val == 0);
    CHECK(arr0[1].val == 1);
    CHECK(arr0[2].val == 23);
    CHECK(arr0[3].val == 2);
    CHECK(arr0[4].val == 3);
    CHECK(arr0[5].val == 4);

    // erase
    arr0.Erase(5);      // erase from end
    CHECK(arr0.Size() == 5);
    CHECK(arr0[0].val == 0);
    CHECK(arr0[1].val == 1);
    CHECK(arr0[2].val == 23);
    CHECK(arr0[3].val == 2);
    CHECK(arr0[4].val == 3);
    CHECK(arr0.begin()[5].val == 1);
    arr0.Erase(0);      // erase from fron
    CHECK(arr0.Size() == 4);
    CHECK(arr0[0].val == 1);
    CHECK(arr0[1].val == 23);
    CHECK(arr0[2].val == 2);
    CHECK(arr0[3].val == 3);
    CHECK(arr0.begin()[4].val == 1);
    arr0.Erase(1);      // erase from middle
    CHECK(arr0.Size() == 3);
    CHECK(arr0[0].val == 1);
    CHECK(arr0[1].val == 2);
    CHECK(arr0[2].val == 3);
    CHECK(arr0.begin()[3].val == 1);

    // full
    CHECK(!arr0.Full());
    InlineArray<int, 3> arr3 = { 1, 2, 3 };
    CHECK(arr3.Full());

    // emplace, reset
    arr0.Clear();
    arr0.Add(5, 6);
    arr0.Add(7, 8);
    CHECK(arr0.Size() == 2);
    CHECK(arr0[0].val == 5);
    CHECK(arr0[0].wal == 6);
    CHECK(arr0[1].val == 7);
    CHECK(arr0[1].wal == 8);
    arr0.Reset();
    CHECK(arr0.Size() == 0);
}

