#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::HTTP::HTTPResponse
    @brief holds the response of a HTTPRequest
    
    see http://tools.ietf.org/html/rfc2616
*/
#include "Core/RefCounted.h"
#include "Core/Containers/Map.h"
#include "Core/String/String.h"
#include "IO/IOStatus.h"
#include "IO/Stream.h"

namespace Oryol {
namespace HTTP {
    
class HTTPResponse : public Core::RefCounted {
    OryolClassPoolAllocDecl(HTTPResponse);
public:
    /// constructor
    HTTPResponse();
    /// destructor
    virtual ~HTTPResponse();
    
    /// set the HTTP status
    void SetStatus(IO::IOStatus::Code status);
    /// get the HTTP status
    IO::IOStatus::Code GetStatus() const;
    /// set response header fields
    void SetFields(const Core::Map<Core::String, Core::String>& fields);
    /// response header fields
    const Core::Map<Core::String, Core::String>& GetFields() const;
    /// set response body
    void SetBody(const Core::Ptr<IO::Stream>& stream);
    /// get response body
    const Core::Ptr<IO::Stream>& GetBody() const;

private:
    IO::IOStatus::Code httpStatus;
    Core::Map<Core::String, Core::String> headerFields;
    Core::Ptr<IO::Stream> body;
};
    
} // namespace HTTP
} // namespace Oryol
 