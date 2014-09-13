#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::HTTP::HTTPFileSystem
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
namespace HTTP {
    
class HTTPFileSystem : public IO::FileSystem {
    OryolClassDecl(HTTPFileSystem);
    OryolClassCreator(HTTPFileSystem);
public:
    /// default constructor
    HTTPFileSystem();
    /// destructor
    virtual ~HTTPFileSystem();

    /// per-frame update
    virtual void DoWork();
    /// called when the IOProtocol::Get message is received
    virtual void onGet(const Ptr<IO::IOProtocol::Get>& msg);
    /// called when the IOProtocol::GetRange message is received
    virtual void onGetRange(const Ptr<IO::IOProtocol::GetRange>& msg);

private:
    StringBuilder stringBuilder;
    Ptr<HTTPClient> httpClient;
    struct pendingRequest {
        Ptr<IO::IOProtocol::Get> ioRequest;
        Ptr<HTTPProtocol::HTTPRequest> httpRequest;
    };
    Array<pendingRequest> pendingRequests;
};
    
} // namespace HTTP
} // namespace Oryol
 