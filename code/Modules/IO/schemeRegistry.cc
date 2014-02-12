//------------------------------------------------------------------------------
//  schemeRegistry.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "schemeRegistry.h"
#include "IO/FileSystem.h"

namespace Oryol {
namespace IO {

OryolGlobalSingletonImpl(schemeRegistry);

using namespace Core;

//------------------------------------------------------------------------------
schemeRegistry::schemeRegistry() {
    this->SingletonEnsureUnique();
}

//------------------------------------------------------------------------------
schemeRegistry::~schemeRegistry() {
    // empty
}

//------------------------------------------------------------------------------
void
schemeRegistry::UnregisterFileSystem(const StringAtom& scheme) {
    this->rwLock.LockWrite();
    o_assert(this->registry.Contains(scheme));
    this->registry.Erase(scheme);
    this->rwLock.UnlockWrite();
}

//------------------------------------------------------------------------------
bool
schemeRegistry::IsFileSystemRegistered(const StringAtom& scheme) const {
    this->rwLock.LockRead();
    bool result = this->registry.Contains(scheme);
    this->rwLock.UnlockRead();
    return result;
}

//------------------------------------------------------------------------------
Ptr<FileSystem>
schemeRegistry::CreateFileSystem(const StringAtom& scheme) const {
    this->rwLock.LockRead();
    o_assert(this->registry.Contains(scheme));
    Ptr<FileSystem> fileSystem(this->registry[scheme]());
    this->rwLock.UnlockRead();
    return fileSystem;
}

//------------------------------------------------------------------------------
Map<StringAtom, Ptr<FileSystem>>
schemeRegistry::CreateFileSystems() const {
    this->rwLock.LockRead();
    Map<StringAtom, Ptr<FileSystem>> result;
    for (const auto& fs : this->registry) {
        // call creator function for each entry
        result.Insert(fs.Key(), fs.Value()());
    }
    this->rwLock.UnlockRead();
    return result;
}

} // namespace IO
} // namespace Oryol