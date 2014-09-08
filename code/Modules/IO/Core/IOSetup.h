#pragma once
//------------------------------------------------------------------------------
/**
    @class IO::IOSetup
    @brief configure the IO system
*/
#include "Core/String/String.h"
#include "Core/Containers/Map.h"
#include "Core/Containers/KeyValuePair.h"
#include "IO/FS/FileSystem.h"
#include <functional>

namespace Oryol {
namespace IO {
    
class IOSetup {
public:
    /// initial assigns
    Core::Map<Core::String, Core::String> Assigns;
    /// initial file systems
    Core::Map<Core::StringAtom, std::function<Core::Ptr<IO::FileSystem>()>> FileSystems;
};
    
} // namespace IO
} // namespace Oryol
