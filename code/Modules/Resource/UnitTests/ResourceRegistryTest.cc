//------------------------------------------------------------------------------
//  ResourceRegistryTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Resource/Registry.h"

using namespace Oryol;
using namespace Oryol::Resource;

TEST(ResourceRegistryTest) {

    const Locator blaLoc("bla");
    const Locator blaSigLoc("bla", 'BLA_');
    const Locator blubLoc("blub");
    const Locator blobLoc = Locator::NonShared("blob");
    const Id blaId(1, 1, 1);
    const Id blaSigId(2, 2, 1);
    const Id blubId(3, 3, 1);
    const Id blobId(4, 4, 1);
    Array<Id> removed;

    Registry reg;
    CHECK(!reg.IsValid());
    reg.Setup(256);
    CHECK(reg.IsValid());
    
    // check the empty registry
    CHECK(!reg.LookupResource(blaLoc).IsValid());
    CHECK(!reg.HasResourceById(blaId));
    CHECK(reg.GetNumResources() == 0);
    
    // add a shared resource
    reg.AddResource(blaLoc, blaId);
    CHECK(reg.GetNumResources() == 1);
    CHECK(reg.GetIdByIndex(0) == blaId);
    CHECK(reg.HasResourceById(blaId));
    CHECK(reg.GetUseCount(blaId) == 1);
    CHECK(reg.GetLocator(blaId) == blaLoc);
    CHECK(reg.GetDependents(blaId).Empty());
    CHECK(reg.LookupResource(blaLoc) == blaId); // this bumps the use count to 2
    CHECK(reg.GetUseCount(blaId) == 2);
    reg.ReleaseResource(blaId, removed);
    CHECK(removed.Empty());
    CHECK(reg.GetUseCount(blaId) == 1);
    
    // add another shared resource (bla with signature)
    reg.AddResource(blaSigLoc, blaSigId);
    CHECK(reg.GetNumResources() == 2);
    CHECK(reg.GetIdByIndex(0) == blaId);
    CHECK(reg.GetIdByIndex(1) == blaSigId);
    CHECK(reg.HasResourceById(blaSigId));
    CHECK(reg.GetUseCount(blaSigId) == 1);
    CHECK(reg.GetLocator(blaSigId) == blaSigLoc);
    CHECK(reg.GetLocator(blaSigId) != blaLoc);
    
    // add a resource with dependent
    Array<Id> deps;
    deps.Add(blaId);
    deps.Add(blaSigId);
    reg.AddResource(blubLoc, blubId, deps);
    CHECK(reg.GetNumResources() == 3);
    CHECK(reg.GetIdByIndex(0) == blaId);
    CHECK(reg.GetIdByIndex(1) == blaSigId);
    CHECK(reg.GetIdByIndex(2) == blubId);
    CHECK(reg.HasResourceById(blubId));
    CHECK(reg.GetUseCount(blubId) == 1);
    CHECK(reg.GetUseCount(blaId) == 2);
    CHECK(reg.GetUseCount(blaSigId) == 2);
    Array<Id> outDeps = reg.GetDependents(blubId);
    CHECK(outDeps.Size() == 2);
    CHECK(outDeps[0] == blaId);
    CHECK(outDeps[1] == blaSigId);
    
    // do some lookup and release and check use counts
    CHECK(reg.LookupResource(blubLoc) == blubId);
    CHECK(reg.GetUseCount(blubId) == 2);
    CHECK(reg.GetUseCount(blaId) == 3);
    CHECK(reg.GetUseCount(blaSigId) == 3);
    CHECK(reg.LookupResource(blaLoc) == blaId);
    CHECK(reg.GetUseCount(blubId) == 2);
    CHECK(reg.GetUseCount(blaId) == 4);
    CHECK(reg.GetUseCount(blaSigId) == 3);
    reg.ReleaseResource(blaId, removed);
    CHECK(removed.Empty());
    CHECK(reg.GetUseCount(blubId) == 2);
    CHECK(reg.GetUseCount(blaId) == 3);
    CHECK(reg.GetUseCount(blaSigId) == 3);
    reg.ReleaseResource(blubId, removed);
    CHECK(reg.GetUseCount(blubId) == 1);
    CHECK(reg.GetUseCount(blaId) == 2);
    CHECK(reg.GetUseCount(blaSigId) == 2);
    
    // now first release the 2 dependent resources, then
    // the 'parent' resource, after all this, the registry should be empty
    // again
    reg.ReleaseResource(blaId, removed);
    CHECK(removed.Empty());
    reg.ReleaseResource(blaSigId, removed);
    CHECK(removed.Empty());
    CHECK(reg.GetUseCount(blubId) == 1);
    CHECK(reg.GetUseCount(blaId) == 1);
    CHECK(reg.GetUseCount(blaSigId) == 1);
    reg.ReleaseResource(blubId, removed);
    CHECK(removed.Size() == 3);
    CHECK(removed.FindIndexLinear(blaId) != InvalidIndex);
    CHECK(removed.FindIndexLinear(blaSigId) != InvalidIndex);
    CHECK(removed.FindIndexLinear(blubId) != InvalidIndex);
    CHECK(reg.GetNumResources() == 0);
    CHECK(!reg.HasResourceById(blaId));
    CHECK(!reg.HasResourceById(blaSigId));
    CHECK(!reg.HasResourceById(blubId));
    CHECK(!reg.LookupResource(blaLoc).IsValid());
    CHECK(!reg.LookupResource(blaSigLoc).IsValid());
    CHECK(!reg.LookupResource(blubLoc).IsValid());
    
    // add a non-shared resource locator, this cannot be looked up,
    // but properly released (but since it will not be shared, it's
    // use-count will always be 1)
    reg.AddResource(blobLoc, blobId);
    CHECK(reg.GetNumResources() == 1);
    CHECK(reg.GetIdByIndex(0) == blobId);
    CHECK(reg.HasResourceById(blobId));
    CHECK(reg.GetUseCount(blobId) == 1);
    CHECK(reg.GetLocator(blobId) == blobLoc);
    CHECK(!reg.LookupResource(blobLoc).IsValid());
    reg.ReleaseResource(blobId, removed);
    CHECK(removed.Size() == 1);
    CHECK(removed[0] == blobId);
    CHECK(reg.GetNumResources() == 0);

    reg.Discard();
}
