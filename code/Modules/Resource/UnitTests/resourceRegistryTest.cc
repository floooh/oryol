//------------------------------------------------------------------------------
//  ResourceRegistryTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Resource/ResourceRegistry.h"

using namespace Oryol;
using namespace Oryol::_priv;

TEST(ResourceRegistryTest) {

    const Locator blaLoc("bla");
    const Locator blaSigLoc("bla", 'BLA_');
    const Locator blobLoc = Locator::NonShared("blob");
    const Id blaId(1, 1, 1);
    const Id blaSigId(2, 2, 1);
    const Id blobId(4, 4, 1);
    Array<Id> removed;

    ResourceRegistry reg;
    CHECK(!reg.IsValid());
    reg.Setup(256);
    CHECK(reg.IsValid());
    
    // check the empty registry
    CHECK(!reg.Lookup(blaLoc).IsValid());
    CHECK(!reg.Contains(blaId));
    CHECK(reg.GetNumResources() == 0);
    
    // add a shared resource
    reg.Add(blaLoc, blaId, 123);
    CHECK(reg.GetNumResources() == 1);
    CHECK(reg.GetIdByIndex(0) == blaId);
    CHECK(reg.Contains(blaId));
    CHECK(reg.GetLocator(blaId) == blaLoc);
    CHECK(reg.GetLabel(blaId) == 123);
    CHECK(reg.Lookup(blaLoc) == blaId);
    removed = reg.Remove(123);
    CHECK(removed.Size() == 1);
    CHECK(removed[0] == blaId);
    CHECK(reg.GetNumResources() == 0);
    
    // add another shared resource (bla with signature)
    reg.Add(blaLoc, blaId, 124);
    reg.Add(blaSigLoc, blaSigId, 124);
    CHECK(reg.GetNumResources() == 2);
    CHECK(reg.GetIdByIndex(0) == blaId);
    CHECK(reg.GetIdByIndex(1) == blaSigId);
    CHECK(reg.Contains(blaSigId));
    CHECK(reg.GetLocator(blaSigId) == blaSigLoc);
    CHECK(reg.GetLocator(blaSigId) != blaLoc);
    CHECK(reg.GetLabel(blaSigId) == 124);
    
    removed = reg.Remove(124);
    CHECK(removed.Size() == 2);
    CHECK(reg.GetNumResources() == 0);
    CHECK(!reg.Contains(blaId));
    CHECK(!reg.Contains(blaSigId));
    CHECK(!reg.Lookup(blaLoc).IsValid());
    CHECK(!reg.Lookup(blaSigLoc).IsValid());
    
    // add a non-shared resource locator, this cannot be looked up,
    // but properly released (but since it will not be shared, it's
    // use-count will always be 1)
    reg.Add(blobLoc, blobId, 125);
    CHECK(reg.GetNumResources() == 1);
    CHECK(reg.GetIdByIndex(0) == blobId);
    CHECK(reg.Contains(blobId));
    CHECK(reg.GetLocator(blobId) == blobLoc);
    CHECK(reg.GetLabel(blobId) == 125);
    CHECK(!reg.Lookup(blobLoc).IsValid());
    removed = reg.Remove(125);
    CHECK(removed.Size() == 1);
    CHECK(reg.GetNumResources() == 0);

    reg.Discard();
}
