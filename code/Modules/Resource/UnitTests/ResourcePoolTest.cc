//------------------------------------------------------------------------------
//  ResourcePoolTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Resource/ResourcePool.h"
#include "Resource/ResourceBase.h"

using namespace Oryol;

class myResource : public ResourceBase {
public:
    int blub = 0;
};

class myResourcePool : public ResourcePool<myResource> { };

TEST(ResourcePoolTest) {
    const uint16_t myResourceType = 12;
    const int poolSize = 256;
    myResourcePool resourcePool;
    resourcePool.Setup(myResourceType, poolSize);
    CHECK(resourcePool.IsValid());
    CHECK(resourcePool.GetNumSlots() == 256);
    CHECK(resourcePool.GetNumFreeSlots() == 256);
    CHECK(resourcePool.GetNumUsedSlots() == 0);
    CHECK(resourcePool.LastAllocSlot == 0);
    
    Id resId = resourcePool.AllocId();
    CHECK(resId.IsValid());
    CHECK(resId.Type == 12);
    CHECK(resId.SlotIndex == 0);
    CHECK(ResourceState::InvalidState == resourcePool.QueryState(resId));
    CHECK(resourcePool.LastAllocSlot == 0);
    
    resourcePool.Assign(resId, ResourceState::Valid);
    CHECK(resourcePool.GetNumFreeSlots() == 255);
    CHECK(resourcePool.GetNumUsedSlots() == 1);
    CHECK(ResourceState::Valid == resourcePool.QueryState(resId));
    
    const myResource* res = resourcePool.Lookup(resId);
    CHECK(nullptr != res);
    CHECK(res->Id == resId);
    CHECK(resourcePool.QueryResourceInfo(resId).State == ResourceState::Valid);

    Id resId1 = resourcePool.AllocId();
    CHECK(resId1.IsValid());
    CHECK(resId1.Type == 12);
    CHECK(resId1.SlotIndex == 1);
    CHECK(ResourceState::InvalidState == resourcePool.QueryState(resId1));
    CHECK(resourcePool.LastAllocSlot == 1);
    
    resourcePool.Assign(resId1, ResourceState::Valid);
    CHECK(resourcePool.GetNumFreeSlots() == 254);
    CHECK(resourcePool.GetNumUsedSlots() == 2);
    CHECK(ResourceState::Valid == resourcePool.QueryState(resId));
    
    const myResource* res1 = resourcePool.Lookup(resId1);
    CHECK(nullptr != res1);
    CHECK(res1->Id == resId1);
    CHECK(resourcePool.QueryResourceInfo(resId1).State == ResourceState::Valid);

    const ResourcePoolInfo poolInfo = resourcePool.QueryPoolInfo();
    CHECK(poolInfo.ResourceType == myResourceType);
    CHECK(poolInfo.NumSlots == 256);
    CHECK(poolInfo.NumUsedSlots == 2);
    CHECK(poolInfo.NumFreeSlots == 254);
    
    resourcePool.Unassign(resId);
    CHECK(resourcePool.GetNumFreeSlots() == 255);
    CHECK(resourcePool.GetNumUsedSlots() == 1);
    CHECK(resourcePool.QueryState(resId) == ResourceState::InvalidState);
    CHECK(resourcePool.LastAllocSlot == 1);

    resourcePool.Unassign(resId1);
    CHECK(resourcePool.GetNumFreeSlots() == 256);
    CHECK(resourcePool.GetNumUsedSlots() == 0);
    CHECK(resourcePool.QueryState(resId1) == ResourceState::InvalidState);
    CHECK(resourcePool.LastAllocSlot == 0);

    resourcePool.Discard();
    CHECK(!resourcePool.IsValid());
}
