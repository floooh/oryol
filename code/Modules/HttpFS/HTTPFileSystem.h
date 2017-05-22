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
#include "IO/FileSystemBase.h"
#include "Core/Creator.h"
#include "HttpFS/private/urlLoader.h"

namespace Oryol {
    
class HTTPFileSystem : public FileSystemBase {
    OryolClassDecl(HTTPFileSystem);
    OryolClassCreator(HTTPFileSystem);
public:
    /// called when IO message should be handled
    virtual void onMsg(const Ptr<IORequest>& ioReq) override;

private:
    _priv::urlLoader loader;
};
    
} // namespace Oryol
 
