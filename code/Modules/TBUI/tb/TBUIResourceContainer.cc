//------------------------------------------------------------------------------
//  TBUIResourceContainer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "TBUIResourceContainer.h"
#include "Core/Core.h"
#include "IO/IO.h"

namespace Oryol {

using namespace _priv;

//------------------------------------------------------------------------------
TBUIResourceContainer::TBUIResourceContainer() :
runLoopId(RunLoop::InvalidId) {
    // empty
}

//------------------------------------------------------------------------------
void
TBUIResourceContainer::setup(const TBUISetup& setup) {
    o_assert_dbg(!this->isValid());
    this->resPool.Setup(0, setup.ResourcePoolSize);
    this->runLoopId = Core::PostRunLoop()->Add([this] {
        this->update();
    });
    resourceContainerBase::setup(setup.ResourceLabelStackCapacity, setup.ResourceRegistryCapacity);
}

//------------------------------------------------------------------------------
void
TBUIResourceContainer::discard() {
    o_assert_dbg(this->isValid());
    
    Core::PostRunLoop()->Remove(this->runLoopId);
    for (const auto& req : this->pendingIORequests) {
        req.ioRequest->SetCancelled();
    }
    this->pendingIORequests.Clear();
    resourceContainerBase::discard();
    this->resPool.Discard();
}

//------------------------------------------------------------------------------
Id
TBUIResourceContainer::Preload(const Locator& loc, const StringAtom& tbResName) {
    o_assert_dbg(this->isValid());

    Id resId = this->registry.Lookup(loc);
    if (resId.IsValid()) {
        o_assert_dbg(this->resNameMap[tbResName] == resId);
        return resId;
    }
    else {
        o_assert_dbg(!this->resNameMap.Contains(tbResName));
        
        idAndRequest req;
        req.id = this->resPool.AllocId();
        req.ioRequest = IO::LoadFile(loc.Location());
        this->resNameMap.Add(tbResName, req.id);
        this->pendingIORequests.Add(req);
        this->registry.Add(loc, req.id, this->peekLabel());
        this->resPool.Assign(req.id, tbResName, ResourceState::Pending);
        return req.id;
    }
}

//------------------------------------------------------------------------------
void
TBUIResourceContainer::update() {
    o_assert_dbg(this->isValid());
    
    for (int32 i = this->pendingIORequests.Size() - 1; i >= 0; i--) {
        const auto& req = this->pendingIORequests[i];
        if (req.ioRequest->Handled()) {
            if (req.ioRequest->GetStatus() == IOStatus::OK) {
                tbResource* res = this->resPool.Lookup(req.id);
                // resource could have been destroyed before loading finished
                if (res) {
                    res->content = req.ioRequest->GetStream();
                    this->resPool.UpdateState(req.id, ResourceState::Valid);
                }
            }
            else {
                this->resPool.UpdateState(req.id, ResourceState::Failed);
            }
            
            // remove entry from array of pending requests
            this->pendingIORequests.Erase(i);
        }
    }
}

//------------------------------------------------------------------------------
void
TBUIResourceContainer::Destroy(ResourceLabel label) {
    o_assert_dbg(this->isValid());
    
    Array<Id> ids = this->registry.Remove(label);
    for (const Id& id : ids) {
        tbResource* res = this->resPool.Lookup(id);
        if (res) {
            res->Clear();
        }
        this->resPool.Unassign(id);
    }
}

//------------------------------------------------------------------------------
Ptr<Stream>
TBUIResourceContainer::lookupById(const Id& resId) const {
    o_assert_dbg(this->isValid());
    
    tbResource* res = this->resPool.Lookup(resId);
    if (res) {
        return res->content;
    }
    else {
        return Ptr<Stream>();
    }
}

//------------------------------------------------------------------------------
Ptr<Stream>
TBUIResourceContainer::lookupByResName(const StringAtom& tbResName) {
    o_assert_dbg(this->isValid());

    const int32 index = this->resNameMap.FindIndex(tbResName);
    if (InvalidIndex != index) {
        Id id = this->resNameMap.ValueAtIndex(index);
        return this->lookupById(id);
    }
    else {
        return Ptr<Stream>();
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
TBUIResourceContainer::QueryState(Id id) const {
    o_assert_dbg(this->isValid());
    return this->resPool.QueryState(id);
}
    
//------------------------------------------------------------------------------
bool
TBUIResourceContainer::FinishedLoading() const {
    o_assert_dbg(this->isValid());
    return this->pendingIORequests.Empty();
}

} // namespace Oryol
