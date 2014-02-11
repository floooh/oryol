#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::IO::FileSystem
    @brief base-class for FileSystem handlers
  
    @todo: FileSystem description!
*/
#include "Core/RefCounted.h"

namespace Oryol {
namespace IO {
    
class FileSystem : public Core::RefCounted {
    OryolClassDecl(FileSystem);
public:
    /// default constructor
    FileSystem();
    /// destructor
    virtual ~FileSystem();

};
    
} // namespace IO
} // namespace Oryol