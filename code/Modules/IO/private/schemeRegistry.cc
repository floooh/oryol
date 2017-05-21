//------------------------------------------------------------------------------
//  schemeRegistry.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "schemeRegistry.h"
#include "IO/FileSystemBase.h"
#include "Core/Core.h"

#if ORYOL_HAS_THREADS
#include <mutex>
static std::mutex lockMutex;
#define SCOPED_LOCK std::lock_guard<std::mutex> lock(lockMutex)
#else
#define SCOPED_LOCK
#endif

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
void
schemeRegistry::RegisterFileSystem(const StringAtom& scheme, std::function<Ptr<FileSystemBase>()> fsCreator) {
    o_assert_dbg(!this->registry.Contains(scheme));
    {
        SCOPED_LOCK;
        this->registry.Add(scheme, fsCreator);
    }
    // create temp FileSystem object on main-thread to call the Init method
    auto fs = this->CreateFileSystem(scheme);
    fs->init(scheme);
}

//------------------------------------------------------------------------------
void
schemeRegistry::UnregisterFileSystem(const StringAtom& scheme) {
    SCOPED_LOCK;
    o_assert_dbg(this->registry.Contains(scheme));
    this->registry.Erase(scheme);
}

//------------------------------------------------------------------------------
bool
schemeRegistry::IsFileSystemRegistered(const StringAtom& scheme) const {
    SCOPED_LOCK;
    bool result = this->registry.Contains(scheme);
    return result;
}

//------------------------------------------------------------------------------
Ptr<FileSystemBase>
schemeRegistry::CreateFileSystem(const StringAtom& scheme) const {
    SCOPED_LOCK;
    o_assert_dbg(this->registry.Contains(scheme));
    Ptr<FileSystemBase> fileSystem(this->registry[scheme]());
    fileSystem->initLane();
    return fileSystem;
}

} // namespace _priv
} // namespace Oryol
