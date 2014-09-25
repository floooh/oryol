#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::HTTPFileSystem
    @ingroup HTTP
    @brief implements a simple HTTP-based filesystem
    @see HTTPClient, FileSystem
    
    @todo: HTTPFileSystem description
*/
#include "IO/FS/FileSystem.h"
#include "HTTP/HTTPProtocol.h"
#include "HTTP/HTTPClient.h"
#include "Core/String/StringBuilder.h"
#include "Core/Creator.h"

namespace Oryol {
    
class HTTPFileSystem : public FileSystem {
    OryolClassDecl(HTTPFileSystem);
    OryolClassCreator(HTTPFileSystem);
public:
    /// default constructor
    HTTPFileSystem();
    /// destructor
    virtual ~HTTPFileSystem();

    /// per-frame update
    virtual void DoWork();
    /// called when the IOProtocol::Request message is received
    virtual void onRequest(const Ptr<IOProtocol::Request>& msg);

private:
    StringBuilder stringBuilder;
    Ptr<HTTPClient> httpClient;
};
    
} // namespace Oryol
 