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
URLBuilder::Build() {
    // FIXME!
    return URL();
}

} // namespace IO
} // namespace Oryol