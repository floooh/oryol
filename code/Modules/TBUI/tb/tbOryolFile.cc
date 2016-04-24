//------------------------------------------------------------------------------
//  tbOryolFile.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "tbOryolFile.h"
#include "IO/IO.h"
#include "TBUI/TBUI.h"
#include "Core/Log.h"

//------------------------------------------------------------------------------
tb::TBFile*
tb::TBFile::Open(const char* filename, TBFileMode mode) {
    o_assert_dbg(filename);

    // make sure that assigns have been resolved
    Oryol::String resolvedPath = Oryol::IO::ResolveAssigns(filename);

    // NOTE: loaded resource will never move and never go away, so it is
    // ok to store raw pointer here
    Oryol::Buffer* data = Oryol::TBUI::resources().lookupResource(resolvedPath.AsCStr());
    if (data && !data->Empty()) {
        Oryol::_priv::tbOryolFile* obj = new Oryol::_priv::tbOryolFile();
        obj->size = data->Size();
        obj->pos = 0;
        obj->content = data->Data();
        return obj;
    }
    else {
        o_error("TBUI: failed to open file '%s', must be preloaded!\n", filename);
        return nullptr;
    }
}

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
tbOryolFile::~tbOryolFile() {
    this->size = 0;
    this->content = nullptr;
}

//------------------------------------------------------------------------------
long
tbOryolFile::Size() {
    return this->size;
}

//------------------------------------------------------------------------------
size_t
tbOryolFile::Read(void* buf, size_t elemSize, size_t count) {
    int numBytes = int(elemSize) * int(count);
    if ((this->pos + numBytes) > this->size) {
        numBytes = this->size - this->pos;
    }
    Memory::Copy(this->content + this->pos, buf, numBytes);
    this->pos += numBytes;
    return numBytes;
}

} // namespace _priv
} // namespace Oryol