#pragma once
//------------------------------------------------------------------------------
/**
    @defgroup LocalFS LocalFS
    @brief access to the local disk file system

    @class Oryol::LocalFileSystem
    @ingroup LocalFS
    @brief FileSystem subclass to access the local host file system
*/
#include "IO/FS/FileSystem.h"
#include "Core/Creator.h"

namespace Oryol {

class LocalFileSystem : public FileSystem {
    OryolClassDecl(LocalFileSystem);
    OryolClassCreator(LocalFileSystem);
public:
    /// called once on main-thread
    virtual void Init() override;
    /// called when the IOProtocol::Read message is received
    virtual void onRead(const Ptr<IOProtocol::Read>& msg) override;
    /// called when the IOProtocol::Write message is received
    virtual void onWrite(const Ptr<IOProtocol::Write>& msg) override;
};

} // namespace Oryol



