#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::IO::schemeRegistry
    @brief: private: associates URL schemes with FileSystem implementation
*/
#include "Core/RefCounted.h"
#include "Core/Singleton.h"
#include "Core/Threading/RWLock.h"
#include "Core/String/StringAtom.h"
#include "Core/Containers/Map.h"
#include "Core/Creator.h"
#include "IO/FS/FileSystem.h"
#include <functional>
#include <type_traits>

namespace Oryol {
namespace IO {

class schemeRegistry : public Core::RefCounted {
    OryolGlobalSingletonDecl(schemeRegistry);
public:
    /// constructor
    schemeRegistry();
    /// destructor
    virtual ~schemeRegistry();
    
    /// associate URL scheme with filesystem
    void RegisterFileSystem(const Core::StringAtom& scheme, Core::CreatorRef<FileSystem> fsCreator);
    /// unregister a filesystem
    void UnregisterFileSystem(const Core::StringAtom& scheme);
    /// test if a filesystem has been registered
    bool IsFileSystemRegistered(const Core::StringAtom& scheme) const;
    /// return a new filesystem instance
    Core::Ptr<FileSystem> CreateFileSystem(const Core::StringAtom& scheme) const;
    
private:
    mutable Core::RWLock rwLock;
    Core::Map<Core::StringAtom, Core::CreatorRef<FileSystem>> registry;
};

} // namespace IO
} // namespace Oryol