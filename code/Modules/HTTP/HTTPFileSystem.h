#pragma once
//------------------------------------------------------------------------------
/**
    @defgroup HTTP HTTP
    @brief HTTP services, and a HTTP based filesystem

    @class Oryol::HTTPFileSystem
    @ingroup HTTP
    @brief implements a simple HTTP-based filesystem
    @see HTTPClient, FileSystem
    
    @todo: HTTPFileSystem description
*/
#include "Core/Containers/Map.h"
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
    /// called per IO-lane
    virtual void InitLane() override;
    /// called when the IOProtocol::Request message is received
    virtual void onRequest(const Ptr<IOProtocol::Request>& msg) override;

private:
    StringBuilder stringBuilder;
    Ptr<HTTPClient> httpClient;
    Map<String, String> requestHeaders;
};
    
} // namespace Oryol
 