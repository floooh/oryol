//------------------------------------------------------------------------------
//  mtlReleaseQueue.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtl_impl.h"
#include "mtlReleaseQueue.h"
#include "Gfx/Core/GfxConfig.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mtlReleaseQueue::freeItem::freeItem() :
frameIndex(0),
obj(nil) {
    // empty
}

//------------------------------------------------------------------------------
mtlReleaseQueue::freeItem::freeItem(int frameIndex_, ORYOL_OBJC_ID obj_) :
frameIndex(frameIndex_),
obj(obj_) {
    // empty
}

//------------------------------------------------------------------------------
mtlReleaseQueue::freeItem::~freeItem() {
    if (nil != this->obj) {
        ORYOL_OBJC_RELEASE(this->obj);
        this->obj = nil;
    }
}

//------------------------------------------------------------------------------
mtlReleaseQueue::mtlReleaseQueue() :
valid(false) {
    this->releaseQueue.Reserve(256);
}

//------------------------------------------------------------------------------
mtlReleaseQueue::~mtlReleaseQueue() {
    o_assert_dbg(!this->isValid());
}

//------------------------------------------------------------------------------
void
mtlReleaseQueue::setup() {
    o_assert_dbg(!this->isValid());
    this->valid = true;
}

//------------------------------------------------------------------------------
void
mtlReleaseQueue::discard() {
    o_assert_dbg(this->isValid());

    // NOTE: this method must be called when safe (no resources still in flight)
    this->releaseQueue.Clear();
    this->valid = false;
}

//------------------------------------------------------------------------------
bool
mtlReleaseQueue::isValid() const {
    return this->valid;
}

//------------------------------------------------------------------------------
void
mtlReleaseQueue::garbageCollect(int frameIndex) {
    o_assert_dbg(this->isValid());
    const int safeNumFrames = GfxConfig::MaxInflightFrames + 1;
    if (frameIndex > safeNumFrames) {
        const int minReleaseFrame = frameIndex - GfxConfig::MaxInflightFrames;
        while (!this->releaseQueue.Empty() && (this->releaseQueue.Front().frameIndex < minReleaseFrame)) {
            this->releaseQueue.Dequeue();
            o_dbg("mtlReleaseQueue: defer-released resource!\n");
        }
    }
}

//------------------------------------------------------------------------------
void
mtlReleaseQueue::releaseDeferred(int frameIndex, ORYOL_OBJC_ID obj) {
    o_assert_dbg(this->isValid());
    this->releaseQueue.Enqueue(frameIndex, obj);
}

} // namespace _priv
} // namespace Oryol
