#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::schemeRegistry
    @ingroup _priv
    @brief: associates URL schemes with FileSystem implementation
*/
#include "Core/String/StringAtom.h"
#include "Core/Containers/Map.h"
#include "Core/Creator.h"
#include "IO/FileSystemBase.h"
#include <functional>
#include <thread>

namespace Oryol {
namespace _priv {

class schemeRegistry {
public:
    /// associate URL scheme with filesystem
    void RegisterFileSystem(const StringAtom& scheme, std::function<Ptr<FileSystemBase>()> fsCreator);
    /// unregister a filesystem
    void UnregisterFileSystem(const StringAtom& scheme);
    /// test if a filesystem has been registered
    bool IsFileSystemRegistered(const StringAtom& scheme) const;
    /// return a new filesystem instance
    Ptr<FileSystemBase> CreateFileSystem(const StringAtom& scheme) const;
    
    Map<StringAtom, std::function<Ptr<FileSystemBase>()>> registry;
};

} // namespace _priv
} // namespace Oryol
