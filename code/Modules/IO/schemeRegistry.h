#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::IO::schemeRegistry
    @brief: private: associates URL schemes with FileSystem implementation
*/
#include "Core/RefCounted.h"
#include "Core/Macros.h"
#include "Core/Threading/RWLock.h"
#include "Core/String/StringAtom.h"
#include "Core/Containers/Map.h"
#include "IO/FileSystem.h"
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
    template<class TYPE> void RegisterFileSystem(const Core::StringAtom& scheme, std::function<TYPE*()> creator);
    /// unregister a filesystem
    void UnregisterFileSystem(const Core::StringAtom& scheme);
    /// test if a filesystem has been registered
    bool IsFileSystemRegistered(const Core::StringAtom& scheme) const;
    /// return a new filesystem instance
    Core::Ptr<FileSystem> CreateFileSystem(const Core::StringAtom& scheme) const;
    
private:
    mutable Core::RWLock rwLock;
    Core::Map<Core::StringAtom, std::function<FileSystem*()>> registry;
};

//------------------------------------------------------------------------------
template<class TYPE> void
schemeRegistry::RegisterFileSystem(const Core::StringAtom& scheme, std::function<TYPE*()> creator) {
    static_assert(std::is_base_of<FileSystem, TYPE>::value, "schemeRegistry::RegisterFileSystem: TYPE must be derived from FileSystem!");
    this->rwLock.LockWrite();
    o_assert(!this->registry.Contains(scheme));
    this->registry.Insert(scheme, creator);
    this->rwLock.UnlockWrite();
}
    
} // namespace IO
} // namespace Oryol