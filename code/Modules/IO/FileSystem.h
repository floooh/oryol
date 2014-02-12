#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::IO::FileSystem
    @brief base-class for FileSystem handlers

    Subclasses of FileSystem provide a specific file-system implementation
    (e.g. HttpFileSystem, HostFileSystem, etc). FileSystem implementations
    are associated with URL schemes through the schemeRegistry 
    global singleton. 
*/
#include "Core/RefCounted.h"
#include "IO/IOProtocol.h"

namespace Oryol {
namespace IO {
    
class FileSystem : public Core::RefCounted {
    OryolClassDecl(FileSystem);
public:
    /// default constructor
    FileSystem();
    /// destructor
    virtual ~FileSystem();
    
    /// per-frame update
    virtual void DoWork();
    /// called when the IOProtocol::Get message is received
    virtual void onGet(const Core::Ptr<IOProtocol::Get>& msg);
    /// called when the IOProtocol::GetRange message is received
    virtual void onGetRange(const Core::Ptr<IOProtocol::GetRange>& msg);
};
    
} // namespace IO
} // namespace Oryol