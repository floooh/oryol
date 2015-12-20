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
    /// called when a IOProtocol::Request message is received
    virtual void onRequest(const Ptr<IOProtocol::Request>& msg) override;
};

} // namespace Oryol



