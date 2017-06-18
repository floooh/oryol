//------------------------------------------------------------------------------
//  SliceTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Containers/Slice.h"

using namespace Oryol;

TEST(SliceTest) {

    const int size=10;
    int arr[size] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    // empty slice 
    Slice<int> slice;
    CHECK(slice.Size() == 0);
    CHECK(slice.Offset() == 0);
    CHECK(slice.Empty());
    CHECK(slice.begin() == nullptr);
    CHECK(slice.end() == nullptr);

    // constructor default params
    Slice<int> slice0(arr, size);
    CHECK(slice0.Size() == 10);
    CHECK(slice0.begin() == &arr[0]);
    CHECK(slice0.end() == &arr[10]);

    // init from offset and size
    Slice<int> slice1(arr, size, 3, 4);
    CHECK(slice1.Size() == 4);
    CHECK(slice1.Offset() == 3);
    CHECK(!slice1.Empty());
    CHECK(slice1[0] == 3);
    CHECK(slice1[1] == 4);
    CHECK(slice1[2] == 5);
    CHECK(slice1[3] == 6);
    CHECK(slice1.begin() == &arr[3]);
    CHECK(slice1.end() == &arr[7]);

    // copy assignment
    slice = slice1;
    CHECK(slice.Size() == 4);
    CHECK(slice.Offset() == 3);
    CHECK(slice[0] == 3);
    CHECK(slice[1] == 4);
    CHECK(slice[2] == 5);
    CHECK(slice[3] == 6);
    CHECK(slice.begin() == &arr[3]);
    CHECK(slice.end() == &arr[7]);

    // copy construction
    Slice<int> slice2(slice1);
    CHECK(slice2.Size() == 4);
    CHECK(slice2.Offset() == 3);
    CHECK(slice2[0] == 3);
    CHECK(slice2[1] == 4);
    CHECK(slice2[2] == 5);
    CHECK(slice2[3] == 6);
    CHECK(slice2.begin() == &arr[3]);
    CHECK(slice2.end() == &arr[7]);

    // create view from view
    Slice<int> slice3(arr, size, 2, size-2);
    Slice<int> slice4 = slice3.MakeSlice(3, 4);
    CHECK(slice4.Size() == 4);
    CHECK(slice4.Offset() == 5);
    CHECK(slice4[0] == 5);
    CHECK(slice4[1] == 6);
    CHECK(slice4[2] == 7);
    CHECK(slice4[3] == 8);

    // make view with ToEnd magic value
    Slice<int> slice5 = slice3.MakeSlice(5, EndOfRange);
    CHECK(slice5.Size() == 3);
    CHECK(slice5.Offset() == 7);
    CHECK(slice5[0] == 7);
    CHECK(slice5[1] == 8);
    CHECK(slice5[2] == 9);

    // Move
    slice5.Move(-7);
    CHECK(slice5.Size() == 3);
    CHECK(slice5.Offset() == 0);
    CHECK(slice5[0] == 0);
    CHECK(slice5[1] == 1);
    CHECK(slice5[2] == 2);
    slice5.Move(+2);
    CHECK(slice5.Size() == 3);
    CHECK(slice5.Offset() == 2);
    CHECK(slice5[0] == 2);
    CHECK(slice5[1] == 3);
    CHECK(slice5[2] == 4);

    // FillGap
    Slice<int> slice6(arr, size, 5, 5);
    slice6.FillGap(7, 2);   // nothing should have changed
    CHECK(slice6.Size() == 5);
    CHECK(slice6.Offset() == 5);
    slice6.FillGap(5, 2);   // nothing should change
    CHECK(slice6.Size() == 5);
    CHECK(slice6.Offset() == 5);
    slice6.FillGap(3, 2);   // slice should now move to offset=3
    CHECK(slice6.Size() == 5);
    CHECK(slice6.Offset() == 3);
    CHECK(slice6[0] == 3);

    // Reset
    Slice<int> slice7(arr, size, 5, 5);
    CHECK(slice7.Size() == 5);
    CHECK(slice7.Offset() == 5);
    slice7.Reset();
    CHECK(slice7.Empty());
    CHECK(slice7.Offset() == 0);
}
