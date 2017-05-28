//------------------------------------------------------------------------------
//  ArrayViewTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Containers/ArrayView.h"

using namespace Oryol;

TEST(ArrayViewTest) {

    int arr[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    // empty array view
    ArrayView<int> view;
    CHECK(view.Size() == 0);
    CHECK(view.Empty());
    CHECK(view.begin() == nullptr);
    CHECK(view.end() == nullptr);

    // array view initialization
    ArrayView<int> view1(&arr[3], 4);
    CHECK(view1.Size() == 4);
    CHECK(!view1.Empty());
    CHECK(view1[0] == 3);
    CHECK(view1[1] == 4);
    CHECK(view1[2] == 5);
    CHECK(view1[3] == 6);
    CHECK(view1.begin() == &arr[3]);
    CHECK(view1.end() == &arr[7]);

    // copy assignment
    view = view1;
    CHECK(view.Size() == 4);
    CHECK(view[0] == 3);
    CHECK(view[1] == 4);
    CHECK(view[2] == 5);
    CHECK(view[3] == 6);
    CHECK(view.begin() == &arr[3]);
    CHECK(view.end() == &arr[7]);

    // copy construction
    ArrayView<int> view2(view1);
    CHECK(view2.Size() == 4);
    CHECK(view2[0] == 3);
    CHECK(view2[1] == 4);
    CHECK(view2[2] == 5);
    CHECK(view2[3] == 6);
    CHECK(view2.begin() == &arr[3]);
    CHECK(view2.end() == &arr[7]);

    // make sure that all 3 views are actually pointing to the same data
    view[1] = 123;
    CHECK(view[1] == 123);
    CHECK(view1[1] == 123);
    CHECK(view2[1] == 123);
}
