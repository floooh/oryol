#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::IO::ioLane
    @brief private: controls one IO lane thread
    
    @todo: IO::ioLane description
*/
#include "Messaging/ThreadedQueue.h"
#include "Core/Containers/Map.h"
#include "Core/String/StringAtom.h"
#include "IO/IOProtocol.h"
#include "IO/FS/FileSystem.h"

namespace Oryol {
namespace IO {

class ioLane : public Messaging::ThreadedQueue {
    OryolClassDecl(ioLane);
public:
    /// constructor
    ioLane();
    /// destructor
    virtual ~ioLane();
    
private:
    /// lookup filesystem for URL
    Core::Ptr<FileSystem> fileSystemForURL(const URL& url);
    /// called in thread on thread-entry
    virtual void onThreadEnter() override;
    /// called in thread before thread is left
    virtual void onThreadLeave() override;
    /// called after messages are processed, and on each tick (if a TickDuration is set)
    virtual void onTick();
    /// callback for IOProtocol::Get
    void onGet(const Core::Ptr<IOProtocol::Get>& msg);
    /// callback for IOProtocol::GetRange
    void onGetRange(const Core::Ptr<IOProtocol::GetRange>& msg);
    /// callback for IOProtocol::notifyFileSystemAdded
    void onNotifyFileSystemAdded(const Core::Ptr<IOProtocol::notifyFileSystemAdded>& msg);
    /// callback for IOProtocol::notifyFileSystemReplaced
    void onNotifyFileSystemReplaced(const Core::Ptr<IOProtocol::notifyFileSystemReplaced>& msg);
    /// callback for IOProtocol::notifyFileSystemRemoved
    void onNotifyFileSystemRemoved(const Core::Ptr<IOProtocol::notifyFileSystemRemoved>& msg);

    Core::Map<Core::StringAtom, Core::Ptr<FileSystem>> fileSystems;
};
    
} // namespace IO
} // namespace Oryol
 