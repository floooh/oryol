#pragma once
//------------------------------------------------------------------------------
/**
    @file ioRequests.h
    @brief IO request classes header
    
    These are all the core IO requests classes. If you want to 
    add custom requests to your own filesystem implementations, please
    add those custom requests in your own module, not here.
*/
#include "Core/Config.h"
#include "Core/RefCounted.h"
#include "Core/Containers/Buffer.h"
#include "IO/IOTypes.h"

namespace Oryol {
namespace _priv {
//------------------------------------------------------------------------------
class ioMsg : public RefCounted {
    OryolClassDecl(ioMsg);
    OryolBaseTypeDecl(ioMsg);
public:
    ioMsg() : Handled(false), Cancelled(false) { };
    #if ORYOL_HAS_ATOMIC
    std::atomic<bool> Handled;
    std::atomic<bool> Cancelled;
    #else
    bool Handled;
    bool Cancelled;
    #endif
};
} // namespace _priv;

//------------------------------------------------------------------------------
class IORequest : public _priv::ioMsg {
    OryolClassDecl(IORequest);
    OryolTypeDecl(IORequest, _priv::ioMsg);
public:
    URL Url;
    int StartOffset = 0;
    int EndOffset = EndOfFile;
    Buffer Data;
    IOStatus::Code Status = IOStatus::InvalidIOStatus;
    String ErrorDesc;
};

//------------------------------------------------------------------------------
class IORead : public IORequest {
    OryolClassDecl(IORead);
    OryolTypeDecl(IORead, IORequest);
public:
    bool CacheReadEnabled = false;
    bool CacheWriteEnabled = false;
};

//------------------------------------------------------------------------------
class IOWrite : public IORequest {
    OryolClassDecl(IOWrite);
    OryolTypeDecl(IOWrite, IORequest);
};

//------------------------------------------------------------------------------
class notifyWorkers : public _priv::ioMsg {
    OryolClassDecl(notifyWorkers);
    OryolTypeDecl(notifyWorkers, ioMsg);
public:
    StringAtom Scheme;
};

//------------------------------------------------------------------------------
class notifyFileSystemRemoved : public notifyWorkers {
    OryolClassDecl(notifyFileSystemRemoved);
    OryolTypeDecl(notifyFileSystemRemoved, notifyWorkers);
};

//------------------------------------------------------------------------------
class notifyFileSystemAdded : public notifyWorkers {
    OryolClassDecl(notifyFileSystemAdded);
    OryolTypeDecl(notifyFileSystemAdded, notifyWorkers);
};

//------------------------------------------------------------------------------
class notifyFileSystemReplaced : public notifyWorkers {
    OryolClassDecl(notifyFileSystemReplaced);
    OryolTypeDecl(notifyFileSystemReplaced, notifyWorkers);
};

} // namespace Oryol
