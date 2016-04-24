#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlReleaseQueue
    @ingroup _priv
    @brief hold Metal freed resource references until no longer used by GPU
*/
#include "Gfx/mtl/mtl_decl.h"
#include "Core/Containers/Queue.h"

namespace Oryol {
namespace _priv {

class mtlReleaseQueue {
public:
    /// constructor
    mtlReleaseQueue();
    /// destructor
    ~mtlReleaseQueue();

    /// setup the object
    void setup();
    /// discard the object (releases remaining refs, must be called when safe!)
    void discard();
    /// return true if object has been setup
    bool isValid() const;
    /// call per-frame to release ids when safe
    void garbageCollect(int frameIndex);

    /// add an id to be released when no longer used by GPU
    void releaseDeferred(int frameIndex, ORYOL_OBJC_ID obj);

    struct freeItem {
        freeItem();
        freeItem(int frameIndex, ORYOL_OBJC_ID obj);
        ~freeItem();

        int frameIndex;
        ORYOL_OBJC_ID obj;
    };
    Queue<freeItem> releaseQueue;
    bool valid;
};

} // namespace _priv
} // namespace Oryol
