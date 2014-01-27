//------------------------------------------------------------------------------
//  URLBuilder.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "URLBuilder.h"

namespace Oryol {
namespace IO {

using namespace Core;
    
//------------------------------------------------------------------------------
URLBuilder::URLBuilder() {
    // empty
}

//------------------------------------------------------------------------------
URLBuilder::URLBuilder(const URL& url) {
    this->SetURL(url);
}

//------------------------------------------------------------------------------
void
URLBuilder::SetURL(const URL& url) {
    this->scheme   = url.Scheme();
    this->user     = url.User();
    this->password = url.Password();
    this->host     = url.Host();
    this->port     = url.Port();
    this->path     = url.Path();
    this->fragment = url.Fragment();
    this->query    = url.Query();
}

//------------------------------------------------------------------------------
URL
URLBuilder::BuildURL() {
    this->stringBuilder.Clear();
    if (this->scheme.IsValid()) {
        this->stringBuilder.Append(this->scheme);
        this->stringBuilder.Append("://");
    }
    if (this->user.IsValid()) {
        this->stringBuilder.Append(this->user);
        if (this->password.IsValid()) {
            this->stringBuilder.Append(':');
            this->stringBuilder.Append(this->password);
        }
        this->stringBuilder.Append('@');
    }
    if (this->host.IsValid()) {
        this->stringBuilder.Append(this->host);
        if (this->port.IsValid()) {
            this->stringBuilder.Append(':');
            this->stringBuilder.Append(this->port);
        }
    }
    this->stringBuilder.Append('/');
    if (this->path.IsValid()) {
        this->stringBuilder.Append(this->path);
    }
    if (!this->query.Empty()) {
        this->stringBuilder.Append('?');
        for (const auto& kvp : this->query) {
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
    if (this->fragment.IsValid()) {
        this->stringBuilder.Append('#');
        this->stringBuilder.Append(this->fragment);
    }
    return URL(this->stringBuilder.GetString());
}

} // namespace IO
} // namespace Oryol