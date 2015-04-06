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
    Oryol::Ptr<Oryol::Stream> data = Oryol::TBUI::resources().lookupResource(resolvedPath.AsCStr());
    if (data.isValid()) {
        data->Open(Oryol::OpenMode::ReadOnly);
        Oryol::_priv::tbOryolFile* obj = new Oryol::_priv::tbOryolFile();
        obj->content = data;
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
    if (this->content.isValid()) {
        this->content->Close();
        this->content.invalidate();
    }
}

//------------------------------------------------------------------------------
long
tbOryolFile::Size() {
    return this->content->Size();
}

//------------------------------------------------------------------------------
size_t
tbOryolFile::Read(void* buf, size_t elemSize, size_t count) {
    return this->content->Read(buf, elemSize * count);
}

} // namespace _priv
} // namespace Oryol