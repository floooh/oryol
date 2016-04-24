#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::tbOryolFile
    @brief file operations wrapper for turbobadger
    
    Turbobadger doesn't support asynchronous IO (yet?), thus all
    files must be preloaded into memory before consumed to turbobadger.
    The Oryol wrapper implements this inside the TBUIResourceContainer class,
    tbOryolFile is the layer between this and turbobadger.
*/
#include "Core/Types.h"
#include "tb_system.h"

namespace Oryol {
namespace _priv {

class tbOryolFile : public tb::TBFile {
public:
    /// destructor
    virtual ~tbOryolFile();
    /// get file size
    virtual long Size();
    /// read data from file
    virtual size_t Read(void* buf, size_t elemSize, size_t count);

    int size = 0;
    int pos = 0;
    uint8_t* content = nullptr;
};

} // namespace _priv
} // namespace Oryol
