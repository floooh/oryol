//------------------------------------------------------------------------------
//  ResourcePoolTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Resource/Core/ResourcePool.h"
#include "Resource/Core/resourceBase.h"
#include "Resource/Core/simpleFactory.h"

using namespace Oryol;

class mySetup {
public:
    mySetup() : bla(0) {};
    mySetup(int32 bla_) : bla(bla_) {};
    int32 bla;
};

class myResource : public resourceBase<mySetup> {
public:
    int32 blub = 0;
};

class myFactory : public simpleFactory<myResource> {
public:
    uint16 GetResourceType() const {
        return 12;
    };
    void SetupResource(myResource& res) {
        res.blub = res.GetSetup().bla;
        res.setState(ResourceState::Valid);
    };
    void DestroyResource(myResource& res) {
        res.blub = 0;
        // FIXME: factory::DestroyResource should have to change the state
        res.setState(ResourceState::Setup);
    };
};

class myResourcePool : public ResourcePool<myResource, mySetup, myFactory> { };

TEST(ResourcePoolTest) {
    myFactory factory;
    myResourcePool resourcePool;
    resourcePool.Setup(&factory, 256, 0, 'XYZW');
    CHECK(resourcePool.IsValid());
    CHECK(resourcePool.GetNumSlots() == 256);
    CHECK(resourcePool.GetNumFreeSlots() == 256);
    CHECK(resourcePool.GetNumUsedSlots() == 0);
    CHECK(resourcePool.GetNumPendingSlots() == 0);
    
    Id resId = resourcePool.AllocId(8);
    CHECK(resId.IsValid());
    CHECK(resId.Label() == 8);
    CHECK(resId.Type() == 12);
    CHECK(resId.SlotIndex() == 0);
    CHECK(ResourceState::InvalidState == resourcePool.QueryState(resId));
    
    resourcePool.Assign(resId, mySetup(123));
    CHECK(resourcePool.GetNumFreeSlots() == 255);
    CHECK(resourcePool.GetNumUsedSlots() == 1);
    CHECK(resourcePool.GetNumPendingSlots() == 0);
    CHECK(ResourceState::Valid == resourcePool.QueryState(resId));
    
    myResource* res = resourcePool.Lookup(resId);
    CHECK(nullptr != res);
    CHECK(res->GetId() == resId);
    CHECK(res->GetState() == ResourceState::Valid);
    CHECK(res->GetSetup().bla == 123);
    CHECK(res->blub == 123);
    
    Id resId1 = resourcePool.AllocId(9);
    CHECK(resId1.IsValid());
    CHECK(resId1.Label() == 9);
    CHECK(resId1.Type() == 12);
    CHECK(resId1.SlotIndex() == 1);
    CHECK(ResourceState::InvalidState == resourcePool.QueryState(resId1));
    
    resourcePool.Assign(resId1, mySetup(12345));
    CHECK(resourcePool.GetNumFreeSlots() == 254);
    CHECK(resourcePool.GetNumUsedSlots() == 2);
    CHECK(resourcePool.GetNumPendingSlots() == 0);
    CHECK(ResourceState::Valid == resourcePool.QueryState(resId));
    
    myResource* res1 = resourcePool.Lookup(resId1);
    CHECK(nullptr != res1);
    CHECK(res1->GetId() == resId1);
    CHECK(res1->GetState() == ResourceState::Valid);
    CHECK(res1->GetSetup().bla == 12345);
    CHECK(res1->blub == 12345);
    
    resourcePool.Unassign(resId);
    CHECK(resourcePool.GetNumFreeSlots() == 255);
    CHECK(resourcePool.GetNumUsedSlots() == 1);
    CHECK(resourcePool.GetNumPendingSlots() == 0);
    // FIXME: this is inconsistent, what should really happen to
    // unassigned slots (well, they will be re-assigned with
    // another resource...)
    CHECK(res->GetState() == ResourceState::Initial);
    CHECK(res->blub == 0);
    CHECK(res->GetSetup().bla == 123);
    
    resourcePool.Unassign(resId1);
    CHECK(resourcePool.GetNumFreeSlots() == 256);
    CHECK(resourcePool.GetNumUsedSlots() == 0);
    CHECK(resourcePool.GetNumPendingSlots() == 0);
    CHECK(res1->GetState() == ResourceState::Initial);
    CHECK(res1->blub == 0);
    CHECK(res1->GetSetup().bla == 12345);
    
    resourcePool.Discard();
    CHECK(!resourcePool.IsValid());
}