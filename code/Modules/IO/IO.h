#pragma once
//------------------------------------------------------------------------------
/**
    @defgroup IO IO
    @brief loading data asynchronously through pluggable filesystems

    @class Oryol::IO
    @ingroup IO
    @brief IO module facade
*/
#include "Core/RefCounted.h"
#include "Core/String/String.h"
#include "Core/String/StringAtom.h"
#include "IO/Core/IOSetup.h"
#include "IO/IOProtocol.h"
#include "IO/FS/ioRequestRouter.h"
#include "IO/Core/assignRegistry.h"
#include "IO/Core/schemeRegistry.h"
#include "Core/RunLoop.h"

namespace Oryol {

class IO {
public:
    /// setup the IO module
    static void Setup(const IOSetup& setup);
    /// discard the IO module
    static void Discard();
    /// check if IO module is valid
    static bool IsValid();
    
    /// add or replace an assign definition
    static void SetAssign(const String& assign, const String& path);
    /// check if an assign exists
    static bool HasAssign(const String& assign);
    /// lookup an assign (return empty string if not exists)
    static String LookupAssign(const String& assign);
    /// resolve assigns in the provided string
    static String ResolveAssigns(const String& str);
    
    /// associate URL scheme with filesystem
    static void RegisterFileSystem(const StringAtom& scheme, std::function<Ptr<FileSystem>()> fsCreator);
    /// unregister a filesystem
    static void UnregisterFileSystem(const StringAtom& scheme);
    /// test if a filesystem has been registered
    static bool IsFileSystemRegistered(const StringAtom& scheme);
    
    /// start async loading of file from URL (also see IOQueue!)
    static Ptr<IOProtocol::Request> LoadFile(const URL& url, int32 ioLane=0);
    /// push a generic asynchronous IO request
    static void Put(const Ptr<IOProtocol::Request>& ioReq);
    
private:
    /// pump the ioRequestRouter
    static void doWork();

    struct _state {
        _priv::assignRegistry assignReg;
        _priv::schemeRegistry schemeReg;
        RunLoop::Id runLoopId = RunLoop::InvalidId;
        Ptr<_priv::ioRequestRouter> requestRouter;
    };
    static _state* state;
};

} // namespace Oryol
