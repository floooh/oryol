//------------------------------------------------------------------------------
//  URLBuilder.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "URLBuilder.h"

namespace Oryol {

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