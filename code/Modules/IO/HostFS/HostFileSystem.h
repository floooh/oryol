#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::HostFileSystem
    @ingroup IO
    @brief host filesystem implementation that uses stdio
*/
#include "IO/FS/FileSystem.h"
#include "IO/HostFS/hostFileAccess.h"
#include "Core/Containers/Queue.h"
#include <cstdio>

namespace Oryol {

class HostFileSystem : public FileSystem {
    OryolClassDecl(HostFileSystem);
public:
    /// default constructor
    HostFileSystem();
    /// destructor
    virtual ~HostFileSystem();

    /// per-frame update
    virtual void DoWork() override;
    /// called when the IOProtocol::Request message is received
    virtual void onRequest(const Ptr<IOProtocol::Request>& msg) override;
private:
    Queue<Ptr<IOProtocol::Request>> requestQueue;
    _priv::hostFileAccess hostFileAccess;
};

} // namespace Oryol
