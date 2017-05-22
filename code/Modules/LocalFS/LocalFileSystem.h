#pragma once
//------------------------------------------------------------------------------
/**
    @defgroup LocalFS LocalFS
    @brief access to the local disk file system

    @class Oryol::LocalFileSystem
    @ingroup LocalFS
    @brief FileSystem subclass to access the local host file system
*/
#include "IO/FileSystemBase.h"
#include "Core/Creator.h"

namespace Oryol {

class LocalFileSystem : public FileSystemBase {
    OryolClassDecl(LocalFileSystem);
    OryolClassCreator(LocalFileSystem);
public:
    /// called once on main-thread
    virtual void init(const StringAtom& scheme) override;
    /// called when IO message should be handled
    virtual void onMsg(const Ptr<IORequest>& ioReq) override;

private:
    /// handle IORead msg
    void onRead(const Ptr<IORead>& ioRead);
    /// handle IOWrite msg
    void onWrite(const Ptr<IOWrite>& ioWrite);
};

} // namespace Oryol



