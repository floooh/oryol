//------------------------------------------------------------------------------
//  IdTest.cc
//  Test resource Id
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Resource/Id.h"

using namespace Oryol;

TEST(IdTest) {
    Id id;
    CHECK(!id.IsValid());
    CHECK(Id::InvalidUniqueStamp == id.UniqueStamp);
    CHECK(Id::InvalidSlotIndex == id.SlotIndex);
    CHECK(Id::InvalidType == id.Type);
    CHECK(id == Id::InvalidId());
    
    Id id1(1, 2, 3);
    CHECK(id1.IsValid());
    CHECK(id != id1);
    CHECK(id1.UniqueStamp == 1);
    CHECK(id1.SlotIndex == 2);
    CHECK(id1.Type == 3);
    
    // copy-constructor
    Id id2(id1);
    CHECK(id1 == id2);
    CHECK(id2.IsValid());
    CHECK(id2.UniqueStamp == 1);
    CHECK(id2.SlotIndex == 2);
    CHECK(id2.Type == 3);
    
    // assignment
    Id id3;
    id3 = id1;
    CHECK(id3 == id1);
    CHECK(id3.IsValid());
    CHECK(id3.UniqueStamp == 1);
    CHECK(id3.SlotIndex == 2);
    CHECK(id3.Type == 3);
    
    // invalidation
    id3.Invalidate();
    CHECK(id == id3);
    CHECK(Id::InvalidUniqueStamp == id3.UniqueStamp);
    CHECK(Id::InvalidSlotIndex == id3.SlotIndex);
    CHECK(Id::InvalidType == id3.Type);
    CHECK(id3 == Id::InvalidId());

    // comparison
    id3 = Id(0, 1, 2);
    CHECK(id3.IsValid());
    CHECK(id3.UniqueStamp == 0);
    CHECK(id3.SlotIndex == 1);
    CHECK(id3.Type == 2);
    CHECK(id3 < id2);
}
