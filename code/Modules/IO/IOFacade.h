#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::IO::IOFacade
    @brief facade singleton of the IO module
 
    @todo: IOFacade description
*/
#include "Core/RefCounted.h"
#include "Core/Singleton.h"
#include "Core/String/String.h"
#include "Core/String/StringAtom.h"
#include "IO/Core/IOSetup.h"
#include "IO/Core/schemeRegistry.h"
#include "IO/IOProtocol.h"
#include "IO/FS/ioRequestRouter.h"
#include <thread>

namespace Oryol {
namespace IO {

class IOFacade {
    OryolGlobalSingletonDecl(IOFacade);
public:
    /// constructor
    IOFacade(const IOSetup& setup);
    /// destructor
    ~IOFacade();
    
    /// add or replace an assign definition
    void SetAssign(const Core::String& assign, const Core::String& path);
    /// check if an assign exists
    bool HasAssign(const Core::String& assign) const;
    /// lookup an assign (return empty string if not exists)
    Core::String LookupAssign(const Core::String& assign) const;
    /// resolve assigns in the provided string
    Core::String ResolveAssigns(const Core::String& str) const;
    
    /// associate URL scheme with filesystem
    void RegisterFileSystem(const Core::StringAtom& scheme, std::function<Core::Ptr<IO::FileSystem>()> fsCreator);
    /// unregister a filesystem
    void UnregisterFileSystem(const Core::StringAtom& scheme);
    /// test if a filesystem has been registered
    bool IsFileSystemRegistered(const Core::StringAtom& scheme) const;
    
    /// start async loading of file from URL (also see IOQueue!)
    Core::Ptr<IO::IOProtocol::Get> LoadFile(const URL& url, int32 ioLane=0);
    /// push a generic asynchronous IO request
    void Put(const Core::Ptr<IO::IOProtocol::Request>& ioReq);
    
private:
    /// test if we are on the main thread
    bool isMainThread();
    /// the per-frame update method (attached to the main-thread runloop)
    void doWork();

    int32 runLoopId;
    std::thread::id mainThreadId;
    Core::Ptr<ioRequestRouter> requestRouter;
    static const int32 numIOLanes;
};

} // namespace IO
} // namespace Oryol