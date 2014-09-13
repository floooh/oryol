#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::FileSystem
    @ingroup IO
    @brief base-class for FileSystem handlers

    Subclasses of FileSystem provide a specific file-system implementation
    (e.g. HttpFileSystem, HostFileSystem, etc). FileSystem implementations
    are associated with URL schemes through the schemeRegistry 
    global singleton. 
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
    virtual ~FileSystem();
    
    /// per-frame update
    virtual void DoWork();
    /// called when the IOProtocol::Get message is received
    virtual void onGet(const Ptr<IOProtocol::Get>& msg);
    /// called when the IOProtocol::GetRange message is received
    virtual void onGetRange(const Ptr<IOProtocol::GetRange>& msg);
};
    
} // namespace Oryol