#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::FileSystemBase
    @ingroup IO
    @brief base-class for FileSystem handlers

    Subclasses of FileSystem provide a specific file-system implementation
    (e.g. HttpFileSystem, HostFileSystem, etc).
*/
#include "Core/String/StringAtom.h"
#include "Core/RefCounted.h"
#include "IO/private/ioRequests.h"

namespace Oryol {
    
class FileSystemBase : public RefCounted {
    OryolClassDecl(FileSystemBase);
public:
    /// destructor
    virtual ~FileSystemBase();

    /// called once on main-thread
    virtual void init(const StringAtom& scheme);
    /// called per IO-lane
    virtual void initLane();
    /// called when IO message should be handled
    virtual void onMsg(const Ptr<IORequest>& ioReq);

    StringAtom scheme;
};
    
} // namespace Oryol
