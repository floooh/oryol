//------------------------------------------------------------------------------
//  IOTypes.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "IOTypes.h"
#include "IO/IO.h"

namespace Oryol {

#define _TOSTRING(c) case c: return #c

//------------------------------------------------------------------------------
const char*
IOStatus::ToString(Code c) {
    switch (c) {
        _TOSTRING(Continue);
        _TOSTRING(SwitchingProtocols);
        _TOSTRING(OK);
        _TOSTRING(Created);
        _TOSTRING(Accepted);
        _TOSTRING(NonAuthorativeInformation);
        _TOSTRING(NoContent);
        _TOSTRING(ResetContent);
        _TOSTRING(PartialContent);
        _TOSTRING(MultipleChoices);
        _TOSTRING(MovedPermanently);
        _TOSTRING(Found);
        _TOSTRING(SeeOther);
        _TOSTRING(NotModified);
        _TOSTRING(UseProxy);
        _TOSTRING(TemporaryRedirect);
        _TOSTRING(BadRequest);
        _TOSTRING(Unauthorized);
        _TOSTRING(PaymentRequired);
        _TOSTRING(Forbidden);
        _TOSTRING(NotFound);
        _TOSTRING(MethodNotAllowed);
        _TOSTRING(NotAcceptable);
        _TOSTRING(ProxyAuthenticationRequired);
        _TOSTRING(RequestTimeout);
        _TOSTRING(Conflict);
        _TOSTRING(Gone);
        _TOSTRING(LengthRequired);
        _TOSTRING(PreconditionFailed);
        _TOSTRING(RequestEntityTooLarge);
        _TOSTRING(RequestURITooLarge);
        _TOSTRING(UnsupportedMediaType);
        _TOSTRING(RequestedRangeNotSatisfiable);
        _TOSTRING(ExpectationFailed);
        _TOSTRING(InternalServerError);
        _TOSTRING(NotImplemented);
        _TOSTRING(BadGateway);
        _TOSTRING(ServiceUnavailable);
        _TOSTRING(GatewayTimeout);
        _TOSTRING(HTTPVersionNotSupported);
        _TOSTRING(Cancelled);
        _TOSTRING(DownloadError);
        default: return "InvalidIOStatus";
    }
}

//------------------------------------------------------------------------------
void
URL::clearIndices() {
    for (int i = 0; i < numIndices; i++) {
        this->indices[i] = InvalidIndex;
    }
}

//------------------------------------------------------------------------------
void
URL::copyIndices(const URL& rhs) {
    for (int i = 0; i < numIndices; i++) {
        this->indices[i] = rhs.indices[i];
    }
}

//------------------------------------------------------------------------------
URL::URL() :
valid(false) {
    this->clearIndices();
}
    
//------------------------------------------------------------------------------
URL::URL(const URL& rhs) :
content(rhs.content),
valid(rhs.valid) {
    this->copyIndices(rhs);
}
    
//------------------------------------------------------------------------------
URL::URL(URL&& rhs) {
    this->content = std::move(rhs.content);
    this->copyIndices(rhs);
    this->valid = rhs.valid;
    rhs.clearIndices();
    rhs.valid = false;
}
    
//------------------------------------------------------------------------------
URL::URL(const char* rhs) :
valid(false) {
    this->crack(rhs);
}
    
//------------------------------------------------------------------------------
URL::URL(const StringAtom& rhs) :
valid(false) {
    this->crack(rhs.AsString());
}
    
//------------------------------------------------------------------------------
URL::URL(const String& rhs) :
content(rhs),
valid(false) {
    this->crack(rhs);
}
    
//------------------------------------------------------------------------------
void
URL::operator=(const URL& rhs) {
    if (&rhs != this) {
        this->content = rhs.content;
        this->copyIndices(rhs);
        this->valid = rhs.valid;
    }
}
    
//------------------------------------------------------------------------------
void
URL::operator=(URL&& rhs) {
    if (&rhs != this) {
        this->content = rhs.content;
        this->copyIndices(rhs);
        this->valid = rhs.valid;
        rhs.content.Clear();
        rhs.clearIndices();
        rhs.valid = false;
    }
}
    
//------------------------------------------------------------------------------
void
URL::operator=(const char* rhs) {
    this->content = rhs;
    this->crack(rhs);
}
    
//------------------------------------------------------------------------------
void
URL::operator=(const StringAtom& rhs) {
    this->content = rhs;
    this->crack(rhs.AsString());
}
    
//------------------------------------------------------------------------------
void
URL::operator=(const String& rhs) {
    this->content = rhs;
    this->crack(rhs);
}
    
//------------------------------------------------------------------------------
bool
URL::operator==(const URL& rhs) {
    return this->content == rhs.content;
}
    
//------------------------------------------------------------------------------
bool
URL::operator!=(const URL& rhs) {
    return this->content != rhs.content;
}
    
//------------------------------------------------------------------------------
const StringAtom&
URL::Get() const {
    return this->content;
}

//------------------------------------------------------------------------------
const char*
URL::AsCStr() const {
    return this->content.AsCStr();
}

//------------------------------------------------------------------------------
bool
URL::IsValid() const {
    return this->valid;
}

//------------------------------------------------------------------------------
bool
URL::Empty() const {
    return !this->content.IsValid();
}

//------------------------------------------------------------------------------
void
URL::crack(String urlString) {

    this->content.Clear();
    this->clearIndices();
    this->valid = false;
    
    if (IO::IsValid()) {
        urlString = IO::ResolveAssigns(urlString);
    }
    if (urlString.IsValid()) {
    
        StringBuilder builder;
        builder.Set(urlString);
        this->content = urlString;
        
        // extract scheme
        this->indices[schemeStart] = 0;
        this->indices[schemeEnd] = builder.FindSubString(0, 8, "://");
        if (EndOfString == this->indices[schemeEnd]) {
            o_warn("URL::crack(): '%s' is not a valid URL!\n", this->content.AsCStr());
            this->clearIndices();
            return;
        }
        
        // extract host fields
        int leftStartIndex = this->indices[schemeEnd] + 3;
        int leftEndIndex = builder.FindFirstOf(leftStartIndex, EndOfString, "/");
        if (EndOfString == leftEndIndex) {
            leftEndIndex = builder.Length();
        }
        if (leftStartIndex != leftEndIndex) {
            // extract user and password
            int userAndPwdEndIndex = builder.FindFirstOf(leftStartIndex, leftEndIndex, "@");
            if (EndOfString != userAndPwdEndIndex) {
                // only user, or user:pwd?
                int userEndIndex = builder.FindFirstOf(leftStartIndex, userAndPwdEndIndex, ":");
                if (EndOfString != userEndIndex) {
                    // user and password
                    this->indices[userStart] = leftStartIndex;
                    this->indices[userEnd]   = userEndIndex;
                    this->indices[pwdStart]  = userEndIndex + 1;
                    this->indices[pwdEnd]    = userAndPwdEndIndex;
                }
                else {
                    // only user
                    this->indices[userStart] = leftStartIndex;
                    this->indices[userEnd]   = userAndPwdEndIndex;
                }
                leftStartIndex = userAndPwdEndIndex + 1;
            }
            
            // extract host and port
            int hostEndIndex = builder.FindFirstOf(leftStartIndex, leftEndIndex, ":");
            if (EndOfString != hostEndIndex) {
                // host and port
                this->indices[hostStart] = leftStartIndex;
                this->indices[hostEnd]   = hostEndIndex;
                this->indices[portStart] = hostEndIndex + 1;
                this->indices[portEnd]   = leftEndIndex;
            }
            else {
                // only host name, no port
                this->indices[hostStart] = leftStartIndex;
                this->indices[hostEnd]   = leftEndIndex;
            }
        }
        
        // is there any path component?
        if (leftEndIndex != builder.Length()) {
            // extract right-hand-side (path, fragment, query)
            int rightStartIndex = leftEndIndex + 1;
            int rightEndIndex = builder.Length();
            
            int pathStartIndex = rightStartIndex;
            int pathEndIndex = builder.FindFirstOf(rightStartIndex, rightEndIndex, "#?");
            if (EndOfString == pathEndIndex) {
                pathEndIndex = rightEndIndex;
            }
            if (pathStartIndex != pathEndIndex) {
                this->indices[pathStart] = pathStartIndex;
                this->indices[pathEnd]   = pathEndIndex;
            }

            // extract query
            if ((pathEndIndex != rightEndIndex) && (builder.At(pathEndIndex) == '?')) {
                int queryStartIndex = pathEndIndex + 1;
                int queryEndIndex = builder.FindFirstOf(queryStartIndex, rightEndIndex, "#");
                if (EndOfString == queryEndIndex) {
                    queryEndIndex = rightEndIndex;
                }
                if (queryStartIndex != queryEndIndex) {
                    this->indices[queryStart] = queryStartIndex;
                    this->indices[queryEnd] = queryEndIndex;
                }
                pathEndIndex = queryEndIndex;
            }
            
            // extract fragment
            if ((pathEndIndex != rightEndIndex) && (builder.At(pathEndIndex) == '#')) {
                int fragStartIndex = pathEndIndex + 1;
                int fragEndIndex = builder.FindFirstOf(fragStartIndex, rightEndIndex, "?");
                if (EndOfString == fragEndIndex) {
                    fragEndIndex = rightEndIndex;
                }
                if (fragStartIndex != fragEndIndex) {
                    this->indices[fragStart] = fragStartIndex;
                    this->indices[fragEnd] = fragEndIndex;
                }
            }
        }
        this->valid = true;
    }
    // fallthrough if valid or empty URL
}

//------------------------------------------------------------------------------
bool
URL::HasScheme() const {
    return InvalidIndex != this->indices[schemeStart];
}

//------------------------------------------------------------------------------
String
URL::Scheme() const {
    if (this->HasScheme()) {
        return String(this->content.AsCStr(), this->indices[schemeStart], this->indices[schemeEnd]);
    }
    else {
        return String();
    }
}

//------------------------------------------------------------------------------
bool
URL::HasUser() const {
    return InvalidIndex != this->indices[userStart];
}

//------------------------------------------------------------------------------
String
URL::User() const {
    if (this->HasUser()) {
        return String(this->content.AsCStr(), this->indices[userStart], this->indices[userEnd]);
    }
    else {
        return String();
    }
}

//------------------------------------------------------------------------------
bool
URL::HasPassword() const {
    return InvalidIndex != this->indices[pwdStart];
}

//------------------------------------------------------------------------------
String
URL::Password() const {
    if (this->HasPassword()) {
        return String(this->content.AsCStr(), this->indices[pwdStart], this->indices[pwdEnd]);
    }
    else {
        return String();
    }
}

//------------------------------------------------------------------------------
bool
URL::HasHost() const {
    return InvalidIndex != this->indices[hostStart];
}

//------------------------------------------------------------------------------
String
URL::Host() const {
    if (this->HasHost()) {
        return String(this->content.AsCStr(), this->indices[hostStart], this->indices[hostEnd]);
    }
    else {
        return String();
    }
}

//------------------------------------------------------------------------------
bool
URL::HasPort() const {
    return InvalidIndex != this->indices[portStart];
}

//------------------------------------------------------------------------------
String
URL::Port() const {
    if (this->HasPort()) {
        return String(this->content.AsCStr(), this->indices[portStart], this->indices[portEnd]);
    }
    else {
        return String();
    }
}

//------------------------------------------------------------------------------
String
URL::HostAndPort() const  {
    if (this->HasHost()) {
        if (this->HasPort()) {
            // URL has host and port definition
            return String(this->content.AsCStr(), this->indices[hostStart], this->indices[portEnd]);
        }
        else {
            // URL only has host
            return String(this->content.AsCStr(), this->indices[hostStart], this->indices[hostEnd]);
        }
    }
    else {
        // no host in URL
        return String();
    }
}

//------------------------------------------------------------------------------
bool
URL::HasPath() const {
    return InvalidIndex != this->indices[pathStart];
}

//------------------------------------------------------------------------------
String
URL::Path() const {
    if (this->HasPath()) {
        return String(this->content.AsCStr(), this->indices[pathStart], this->indices[pathEnd]);
    }
    else {
        return String();
    }
}

//------------------------------------------------------------------------------
bool
URL::HasFragment() const {
    return InvalidIndex != this->indices[fragStart];
}

//------------------------------------------------------------------------------
String
URL::Fragment() const {
    if (this->HasFragment()) {
        return String(this->content.AsCStr(), this->indices[fragStart], this->indices[fragEnd]);
    }
    else {
        return String();
    }
}

//------------------------------------------------------------------------------
String
URL::PathToEnd() const {
    if (this->HasPath()) {
        return String(this->content.AsCStr(), this->indices[pathStart], EndOfString);
    }
    else {
        return String();
    }
}

//------------------------------------------------------------------------------
bool
URL::HasQuery() const {
    return InvalidIndex != this->indices[queryStart];
}

//------------------------------------------------------------------------------
Map<String, String>
URL::Query() const {
    if (this->HasQuery()) {
        Map<String, String> query;
        StringBuilder builder;
        builder.Set(this->content.AsCStr(), this->indices[queryStart], this->indices[queryEnd]);
        int kvpStartIndex = 0;
        int kvpEndIndex = 0;
        do {
            kvpEndIndex = builder.FindFirstOf(kvpStartIndex, EndOfString, "&");
            int keyEndIndex = builder.FindFirstOf(kvpStartIndex, kvpEndIndex, "=");
            if (EndOfString != keyEndIndex) {
                // key and value
                String key(builder.GetSubString(kvpStartIndex, keyEndIndex));
                String value(builder.GetSubString(keyEndIndex + 1, kvpEndIndex));
                query.Add(key, value);
            }
            else {
                // only key
                String key(builder.GetSubString(kvpStartIndex, kvpEndIndex));
                query.Add(key, String());
            }
            kvpStartIndex = kvpEndIndex + 1;
        }
        while (EndOfString != kvpEndIndex);
        return query;
    }
    else {
        return Map<String, String>();
    }
}

//------------------------------------------------------------------------------
URLBuilder::URLBuilder() {
    // empty
}

//------------------------------------------------------------------------------
URLBuilder::URLBuilder(const URL& url) {
    this->ParseURL(url);
}

//------------------------------------------------------------------------------
void
URLBuilder::ParseURL(const URL& url) {
    this->Scheme   = url.Scheme();
    this->User     = url.User();
    this->Password = url.Password();
    this->Host     = url.Host();
    this->Port     = url.Port();
    this->Path     = url.Path();
    this->Fragment = url.Fragment();
    this->Query    = url.Query();
}

//------------------------------------------------------------------------------
URL
URLBuilder::BuildURL() {
    this->stringBuilder.Clear();
    if (this->Scheme.IsValid()) {
        this->stringBuilder.Append(this->Scheme);
        this->stringBuilder.Append("://");
    }
    if (this->User.IsValid()) {
        this->stringBuilder.Append(this->User);
        if (this->Password.IsValid()) {
            this->stringBuilder.Append(':');
            this->stringBuilder.Append(this->Password);
        }
        this->stringBuilder.Append('@');
    }
    if (this->Host.IsValid()) {
        this->stringBuilder.Append(this->Host);
        if (this->Port.IsValid()) {
            this->stringBuilder.Append(':');
            this->stringBuilder.Append(this->Port);
        }
    }
    this->stringBuilder.Append('/');
    if (this->Path.IsValid()) {
        this->stringBuilder.Append(this->Path);
    }
    if (!this->Query.Empty()) {
        this->stringBuilder.Append('?');
        for (const auto& kvp : this->Query) {
            this->stringBuilder.Append(kvp.Key());
            if (kvp.Value().IsValid()) {
                this->stringBuilder.Append('=');
                this->stringBuilder.Append(kvp.Value());
            }
            this->stringBuilder.Append("&");
        }
        // remove the last '&'
        this->stringBuilder.PopBack();
    }
    if (this->Fragment.IsValid()) {
        this->stringBuilder.Append('#');
        this->stringBuilder.Append(this->Fragment);
    }
    return URL(this->stringBuilder.GetString());
}

} // namespace Oryol

