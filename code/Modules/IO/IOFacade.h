#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::IO::IOFacade
    @brief facade singleton of the IO module
 
    @todo: IOFacade description
*/
#include "Core/RefCounted.h"
#include "Core/Macros.h"
#include "Core/String/String.h"
#include "Core/String/StringAtom.h"
#include "IO/schemeRegistry.h"
#include "IO/IOProtocol.h"
#include "IO/ioRequestRouter.h"
#include <thread>

namespace Oryol {
namespace IO {

class IOFacade {
    OryolGlobalSingletonDecl(IOFacade);
public:
    /// constructor
    IOFacade();
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
    template<class TYPE> void RegisterFileSystem(const Core::StringAtom& scheme, std::function<Core::Ptr<TYPE>()> creator);
    /// unregister a filesystem
    void UnregisterFileSystem(const Core::StringAtom& scheme);
    /// test if a filesystem has been registered
    bool IsFileSystemRegistered(const Core::StringAtom& scheme) const;
    
    /// asynchronously load a file, returns IORequest object
    Core::Ptr<IOProtocol::Get> LoadFile(const IO::URL& url, int32 ioLane = 0);
    /// asynchronously load a file, return IORequest object
    Core::Ptr<IOProtocol::GetRange> LoadFileRange(const IO::URL& url, int32 startOffset, int32 endOffset, int32 ioLane = 0);
    /// add a preload file
    void AddPreloadFile(const IO::URL& url, int32 ioLane = 0);
    /// return true if preloading is finished
    bool IsPreloadingFinished() const;
    
private:
    /// test if we are on the main thread
    bool isMainThread();
    /// the per-frame update method (attached to the main-thread runloop)
    void doWork();

    std::thread::id mainThreadId;
    Core::Ptr<ioRequestRouter> requestRouter;
    static const int32 numIOLanes;
};

//------------------------------------------------------------------------------
template<class TYPE> void
IOFacade::RegisterFileSystem(const Core::StringAtom& scheme, std::function<Core::Ptr<TYPE>()> creator) {
    schemeRegistry* reg = schemeRegistry::Instance();
    bool newFileSystem = !reg->IsFileSystemRegistered(scheme);
    reg->RegisterFileSystem<TYPE>(scheme, creator);
    if (newFileSystem) {
        // notify IO threads that a filesystem was added
        Core::Ptr<IOProtocol::notifyFileSystemAdded> msg = IOProtocol::notifyFileSystemAdded::Create();
        msg->SetScheme(scheme);
        this->requestRouter->Put(msg);
    }
    else {
        // notify IO threads that a filesystem was replaced
        Core::Ptr<IOProtocol::notifyFileSystemReplaced> msg = IOProtocol::notifyFileSystemReplaced::Create();
        msg->SetScheme(scheme);
        this->requestRouter->Put(msg);
    }
}
    
} // namespace IO
} // namespace Oryol