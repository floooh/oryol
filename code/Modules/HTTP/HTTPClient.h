#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::HTTP::HTTPClient
    @brief message port which implements an HTTP client
    
    A HTTPClient accepts HTTPRequest messages in its Put() method 
    and processes them. The requests may be processed synchronously
    in DoWork, so invoking DoWork can take a long time (this depends 
    on the platform and implementation of the URLLoader class).
    
    Usually a HTTPClient is attached as forwarding port
    to a ThreadedQueue port, so that (1) processing happens in a thread,
    and (2) behaviour is always truly asynchronous, regardless of the platform.
    Internally, HTTPClient uses an urlLoader object which implements
    the platform-specific behaviour.
*/
#include "Messaging/Port.h"
#include "HTTP/HTTPProtocol.h"
#include "HTTP/urlLoader.h"

namespace Oryol {
namespace HTTP {

class HTTPClient : public Messaging::Port {
    OryolClassDecl(HTTPClient);
public:
    /// constructor
    HTTPClient();
    /// destructor
    virtual ~HTTPClient();
    
    /// put a message into the port
    virtual bool Put(const Core::Ptr<Messaging::Message>& msg) override;
    /// perform work
    virtual void DoWork() override;
    
private:
    urlLoader loader;
};
    
} // namespace HTTP
} // namespace Oryol

 