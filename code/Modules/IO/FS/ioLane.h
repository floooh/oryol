#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::ioLane
    @ingroup _priv
    @brief controls one IO lane thread
    
    @todo: ioLane description
*/
#include "Messaging/ThreadedQueue.h"
#include "Core/Containers/Map.h"
#include "Core/String/StringAtom.h"
#include "IO/IOProtocol.h"
#include "IO/FS/FileSystem.h"

namespace Oryol {
namespace _priv {

class ioLane : public ThreadedQueue {
    OryolClassDecl(ioLane);
public:
    /// constructor
    ioLane();
    /// destructor
    virtual ~ioLane();
    
private:
    /// lookup filesystem for URL
    Ptr<FileSystem> fileSystemForURL(const URL& url);
    /// called in thread on thread-entry
    virtual void onThreadEnter() override;
    /// called in thread before thread is left
    virtual void onThreadLeave() override;
    /// called after messages are processed, and on each tick (if a TickDuration is set)
    virtual void onTick();
    /// callback for IOProtocol::Get
    void onGet(const Ptr<IOProtocol::Get>& msg);
    /// callback for IOProtocol::GetRange
    void onGetRange(const Ptr<IOProtocol::GetRange>& msg);
    /// callback for IOProtocol::notifyFileSystemAdded
    void onNotifyFileSystemAdded(const Ptr<IOProtocol::notifyFileSystemAdded>& msg);
    /// callback for IOProtocol::notifyFileSystemReplaced
    void onNotifyFileSystemReplaced(const Ptr<IOProtocol::notifyFileSystemReplaced>& msg);
    /// callback for IOProtocol::notifyFileSystemRemoved
    void onNotifyFileSystemRemoved(const Ptr<IOProtocol::notifyFileSystemRemoved>& msg);

    Map<StringAtom, Ptr<FileSystem>> fileSystems;
};
    
} // namespace _priv
} // namespace Oryol
 