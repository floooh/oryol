//------------------------------------------------------------------------------
//  LocatorTest.cc
//  Test resource Locator
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Resource/Locator.h"

using namespace Oryol;

TEST(Locatortest) {
    
    Locator loc;
    CHECK(!loc.HasValidLocation());
    CHECK(loc.IsShared());
    CHECK(!loc.Location().IsValid());
    CHECK(loc.Signature() == Locator::DefaultSignature);
    
    Locator loc1("tex:ui/bla.dds");
    CHECK(loc1.HasValidLocation());
    CHECK(loc1.IsShared());
    CHECK(loc != loc1);
    CHECK(loc1.Location() == "tex:ui/bla.dds");
    CHECK(loc1.Signature() == Locator::DefaultSignature);
    
    // check with identical location but different signature
    Locator loc2("tex:ui/bla.dds", 123);
    CHECK(loc2.HasValidLocation());
    CHECK(loc2.IsShared());
    CHECK(loc2 != loc1);
    CHECK(loc1.Location() == loc2.Location());
    CHECK(loc2.Signature() == 123);
    
    // copy-constructor
    Locator loc3(loc2);
    CHECK(loc2 == loc3);
    CHECK(loc2.IsShared());
    CHECK(loc3.Location() == loc2.Location());
    CHECK(loc3.Signature() == loc2.Signature());
    
    // create non-shared locator, no location
    Locator loc4 = Locator::NonShared();
    CHECK(loc4 != loc);
    CHECK(!loc4.IsShared());
    CHECK(!loc4.HasValidLocation());
    CHECK(loc4.Signature() == Locator::NonSharedSignature);
    
    // create non-shared locator with location
    Locator loc5 = Locator::NonShared("tex:ui/bla.dds");
    CHECK(loc5 != loc2);
    CHECK(loc5 != loc1);
    CHECK(loc5.Location() == loc1.Location());
    CHECK(!loc5.IsShared());
    CHECK(loc5 != loc4);
}
