#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::schemeRegistry
    @ingroup _priv
    @brief: associates URL schemes with FileSystem implementation
*/
#include "Core/RefCounted.h"
#include "Core/Threading/RWLock.h"
#include "Core/String/StringAtom.h"
#include "Core/Containers/Map.h"
#include "Core/Creator.h"
#include "IO/FS/FileSystem.h"
#include <functional>

namespace Oryol {
namespace _priv {

class schemeRegistry : public RefCounted {
public:
    /// constructor
    schemeRegistry();
    /// destructor
    virtual ~schemeRegistry();
    
    /// associate URL scheme with filesystem
    void RegisterFileSystem(const StringAtom& scheme, std::function<Ptr<FileSystem>()> fsCreator);
    /// unregister a filesystem
    void UnregisterFileSystem(const StringAtom& scheme);
    /// test if a filesystem has been registered
    bool IsFileSystemRegistered(const StringAtom& scheme) const;
    /// return a new filesystem instance
    Ptr<FileSystem> CreateFileSystem(const StringAtom& scheme) const;
    
private:
    mutable RWLock rwLock;
    Map<StringAtom, std::function<Ptr<FileSystem>()>> registry;
};

} // namespace _priv
} // namespace Oryol