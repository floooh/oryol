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
#include "IO/Core/ioPointers.h"

namespace Oryol {
namespace _priv {

class ioLane : public ThreadedQueue {
    OryolClassDecl(ioLane);
public:
    /// constructor
    ioLane(const ioPointers& ptrs);
    /// destructor
    ~ioLane();
    
private:
    /// lookup filesystem for URL
    Ptr<FileSystem> fileSystemForURL(const URL& url);
    /// called in thread on thread-entry
    virtual void onThreadEnter() override;
    /// called in thread before thread is left
    virtual void onThreadLeave() override ;
    /// callback for IOProtocol::Request
    void onRequest(const Ptr<IOProtocol::Request>& msg);
    /// callback for IOProtocol::notifyFileSystemAdded
    void onNotifyFileSystemAdded(const Ptr<IOProtocol::notifyFileSystemAdded>& msg);
    /// callback for IOProtocol::notifyFileSystemReplaced
    void onNotifyFileSystemReplaced(const Ptr<IOProtocol::notifyFileSystemReplaced>& msg);
    /// callback for IOProtocol::notifyFileSystemRemoved
    void onNotifyFileSystemRemoved(const Ptr<IOProtocol::notifyFileSystemRemoved>& msg);

    ioPointers pointers;
    Map<StringAtom, Ptr<FileSystem>> fileSystems;
};
    
} // namespace _priv
} // namespace Oryol
 