//------------------------------------------------------------------------------
//  HashSetTest.cc
//  Test HashSet functionality.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Containers/HashSet.h"

using namespace Oryol;

struct IntHasher {
    uint32_t operator()(int val) {
        return val;
    };
};

TEST(HashSetTest) {
    
    HashSet<int, IntHasher, 4> hashSet;
    CHECK(hashSet.GetMinGrow() == ORYOL_CONTAINER_DEFAULT_MIN_GROW);
    CHECK(hashSet.GetMaxGrow() == ORYOL_CONTAINER_DEFAULT_MAX_GROW);
    CHECK(hashSet.Size() == 0);
    CHECK(hashSet.Empty());
    CHECK(!hashSet.Contains(2));
    
    // Insert and Contains
    hashSet.Add(1);
    hashSet.Add(1024);
    hashSet.Add(12345);
    hashSet.Add(3);
    hashSet.Add(10);
    hashSet.Add(134);
    hashSet.Add(12346);
    hashSet.Add(-4);
    CHECK(hashSet.Size() == 8);
    CHECK(!hashSet.Empty());
    CHECK(hashSet.Contains(1));
    CHECK(hashSet.Contains(1024));
    CHECK(hashSet.Contains(12345));
    CHECK(hashSet.Contains(3));
    CHECK(hashSet.Contains(10));
    CHECK(hashSet.Contains(134));
    CHECK(hashSet.Contains(12346));
    CHECK(hashSet.Contains(-4));
    CHECK(!hashSet.Contains(123));
    
    // copy-construction
    HashSet<int, IntHasher, 4> hashSet1(hashSet);
    CHECK(hashSet1.Size() == 8);
    CHECK(!hashSet1.Empty());
    CHECK(hashSet1.Contains(1));
    CHECK(hashSet1.Contains(1024));
    CHECK(hashSet1.Contains(12345));
    CHECK(hashSet1.Contains(3));
    CHECK(hashSet1.Contains(10));
    CHECK(hashSet1.Contains(134));
    CHECK(hashSet1.Contains(12346));
    CHECK(hashSet1.Contains(-4));
    CHECK(!hashSet1.Contains(123));
    
    // copy-assignment
    HashSet<int, IntHasher, 4> hashSet2;
    hashSet2 = hashSet;
    CHECK(hashSet2.Size() == 8);
    CHECK(!hashSet2.Empty());
    CHECK(hashSet2.Contains(1));
    CHECK(hashSet2.Contains(1024));
    CHECK(hashSet2.Contains(12345));
    CHECK(hashSet2.Contains(3));
    CHECK(hashSet2.Contains(10));
    CHECK(hashSet2.Contains(134));
    CHECK(hashSet2.Contains(12346));
    CHECK(hashSet2.Contains(-4));
    CHECK(!hashSet2.Contains(123));
    
    // move-construction
    HashSet<int, IntHasher, 4> hashSet3(std::move(hashSet2));
    CHECK(hashSet2.Size() == 0);
    CHECK(hashSet2.Empty());
    CHECK(hashSet3.Size() == 8);
    CHECK(!hashSet3.Empty());
    CHECK(hashSet3.Contains(1));
    CHECK(hashSet3.Contains(1024));
    CHECK(hashSet3.Contains(12345));
    CHECK(hashSet3.Contains(3));
    CHECK(hashSet3.Contains(10));
    CHECK(hashSet3.Contains(134));
    CHECK(hashSet3.Contains(12346));
    CHECK(hashSet3.Contains(-4));
    CHECK(!hashSet3.Contains(123));
    
    // move-assignment
    HashSet<int, IntHasher, 4> hashSet4;
    hashSet4 = std::move(hashSet3);
    CHECK(hashSet3.Size() == 0);
    CHECK(hashSet3.Empty());
    CHECK(hashSet4.Size() == 8);
    CHECK(!hashSet4.Empty());
    CHECK(hashSet4.Contains(1));
    CHECK(hashSet4.Contains(1024));
    CHECK(hashSet4.Contains(12345));
    CHECK(hashSet4.Contains(3));
    CHECK(hashSet4.Contains(10));
    CHECK(hashSet4.Contains(134));
    CHECK(hashSet4.Contains(12346));
    CHECK(hashSet4.Contains(-4));
    CHECK(!hashSet4.Contains(123));
    
    // Erase
    hashSet4.Erase(134);
    CHECK(hashSet4.Size() == 7);
    CHECK(!hashSet4.Contains(134));
    CHECK(hashSet4.Contains(1));
    CHECK(hashSet4.Contains(1024));
    CHECK(hashSet4.Contains(12345));
    CHECK(hashSet4.Contains(3));
    CHECK(hashSet4.Contains(10));
    CHECK(hashSet4.Contains(12346));
    CHECK(hashSet4.Contains(-4));
    hashSet4.Erase(1);
    hashSet4.Erase(12345);
    hashSet4.Erase(-4);
    CHECK(hashSet4.Size() == 4);
    CHECK(!hashSet4.Contains(134));
    CHECK(!hashSet4.Contains(1));
    CHECK(hashSet4.Contains(1024));
    CHECK(!hashSet4.Contains(12345));
    CHECK(hashSet4.Contains(3));
    CHECK(hashSet4.Contains(10));
    CHECK(hashSet4.Contains(12346));
    CHECK(!hashSet4.Contains(-4));
    hashSet4.Erase(1024);
    hashSet4.Erase(3);
    hashSet4.Erase(10);
    hashSet4.Erase(12346);
    CHECK(hashSet4.Empty());
    CHECK(hashSet4.Size() == 0);
    CHECK(!hashSet4.Contains(10));
}
