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
    
    /// per-frame update
    virtual void DoWork();
    /// called when the IOProtocol::Request message is received
    virtual void onRequest(const Ptr<IOProtocol::Request>& msg);
};
    
} // namespace Oryol