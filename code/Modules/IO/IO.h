#pragma once
//------------------------------------------------------------------------------
/**
    @defgroup IO IO
    @brief loading data asynchronously through pluggable filesystems

    @class Oryol::IO
    @ingroup IO
    @brief IO module facade
*/
#include "Core/String/String.h"
#include "Core/String/StringAtom.h"
#include "IO/IOTypes.h"
#include "IO/private/loadQueue.h"

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
    static void RegisterFileSystem(const StringAtom& scheme, std::function<Ptr<FileSystemBase>()> fsCreator);
    /// unregister a filesystem
    static void UnregisterFileSystem(const StringAtom& scheme);
    /// test if a filesystem has been registered
    static bool IsFileSystemRegistered(const StringAtom& scheme);

    /// success-callback for Load()
    typedef loadQueue::successFunc LoadSuccessFunc;
    /// success-callback for LoadGroup()
    typedef loadQueue::groupSuccessFunc LoadGroupSuccessFunc;
    /// failed-callback for Load functions
    typedef loadQueue::failFunc LoadFailedFunc;
    /// result of an asynchronous loading operation
    typedef loadQueue::result LoadResult;
    
    /// async load a file, with success and fail callbacks
    static void Load(const URL& url, LoadSuccessFunc onSuccess, LoadFailedFunc onFailed=LoadFailedFunc());
    /// async load a group of files, with success and fail callbacks
    static void LoadGroup(const Array<URL>& urls, LoadGroupSuccessFunc onSuccess, LoadFailedFunc onFailed=LoadFailedFunc());
    /// get number of pending Load() and LoadGroup() actions
    static int NumPendingLoads();

    /// low-level: start async loading of file from URL, return message for polling result
    static Ptr<IORead> LoadFile(const URL& url);
    /// low-level: start async writing of file via URL, return message for polling result
    static Ptr<IOWrite> WriteFile(const URL& url, const Buffer& data);
    /// low-level: push a generic asynchronous IO request
    static void Put(const Ptr<IORequest>& ioReq);
    
private:
    /// pump the ioRequestRouter
    static void doWork();
};

} // namespace Oryol
