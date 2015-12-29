#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::FileSystem
    @ingroup IO
    @brief base-class for FileSystem handlers

    Subclasses of FileSystem provide a specific file-system implementation
    (e.g. HttpFileSystem, HostFileSystem, etc).
*/
#include "Core/RefCounted.h"
#include "IO/IOProtocol.h"

namespace Oryol {
    
class FileSystem : public RefCounted {
    OryolClassDecl(FileSystem);
public:
    /// default constructor
    FileSystem();
    /// destructor
    ~FileSystem();

    /// called once on main-thread
    virtual void Init();
    /// called per IO-lane
    virtual void InitLane();
    /// called when a IOProtocol::Read message is received
    virtual void onRead(const Ptr<IOProtocol::Read>& msg);
    /// called when a IOProtocol::Write message is received
    virtual void onWrite(const Ptr<IOProtocol::Write>& msg);
};
    
} // namespace Oryol