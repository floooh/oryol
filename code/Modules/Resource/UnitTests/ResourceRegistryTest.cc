//------------------------------------------------------------------------------
//  ResourceRegistryTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Resource/Core/ResourceRegistry.h"

using namespace Oryol;

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
    CHECK(!reg.LookupResource(blaLoc).IsValid());
    CHECK(!reg.HasResourceById(blaId));
    CHECK(reg.GetNumResources() == 0);
    
    // add a shared resource
    reg.AddResource(blaLoc, blaId);
    reg.UseResource(blaId);
    CHECK(reg.GetNumResources() == 1);
    CHECK(reg.GetIdByIndex(0) == blaId);
    CHECK(reg.HasResourceById(blaId));
    CHECK(reg.GetUseCount(blaId) == 1);
    CHECK(reg.GetLocator(blaId) == blaLoc);
    CHECK(reg.LookupResource(blaLoc) == blaId);
    reg.UseResource(blaId);
    CHECK(reg.GetUseCount(blaId) == 2);
    CHECK(!reg.ReleaseResource(blaId));
    CHECK(reg.GetUseCount(blaId) == 1);
    
    // add another shared resource (bla with signature)
    reg.AddResource(blaSigLoc, blaSigId);
    reg.UseResource(blaSigId);
    CHECK(reg.GetNumResources() == 2);
    CHECK(reg.GetIdByIndex(0) == blaId);
    CHECK(reg.GetIdByIndex(1) == blaSigId);
    CHECK(reg.HasResourceById(blaSigId));
    CHECK(reg.GetUseCount(blaSigId) == 1);
    CHECK(reg.GetLocator(blaSigId) == blaSigLoc);
    CHECK(reg.GetLocator(blaSigId) != blaLoc);
    
    CHECK(reg.ReleaseResource(blaId));
    CHECK(reg.ReleaseResource(blaSigId));
    CHECK(reg.GetNumResources() == 0);
    CHECK(!reg.HasResourceById(blaId));
    CHECK(!reg.HasResourceById(blaSigId));
    CHECK(!reg.LookupResource(blaLoc).IsValid());
    CHECK(!reg.LookupResource(blaSigLoc).IsValid());
    
    // add a non-shared resource locator, this cannot be looked up,
    // but properly released (but since it will not be shared, it's
    // use-count will always be 1)
    reg.AddResource(blobLoc, blobId);
    reg.UseResource(blobId);
    CHECK(reg.GetNumResources() == 1);
    CHECK(reg.GetIdByIndex(0) == blobId);
    CHECK(reg.HasResourceById(blobId));
    CHECK(reg.GetUseCount(blobId) == 1);
    CHECK(reg.GetLocator(blobId) == blobLoc);
    CHECK(!reg.LookupResource(blobLoc).IsValid());
    CHECK(reg.ReleaseResource(blobId));
    CHECK(reg.GetNumResources() == 0);

    reg.Discard();
}
