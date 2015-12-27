//------------------------------------------------------------------------------
//  dummyFSWrapper.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "dummyFSWrapper.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
dummyFSWrapper::handle 
dummyFSWrapper::openRead(const char* path) { 
    return invalidHandle;
}

//------------------------------------------------------------------------------
dummyFSWrapper::handle 
dummyFSWrapper::openWrite(const char* path) {
    return invalidHandle;
}

//------------------------------------------------------------------------------
int32
dummyFSWrapper::write(handle f, const void* ptr, int32 numBytes) {
    return 0;
}

//------------------------------------------------------------------------------
int32
dummyFSWrapper::read(handle f, void* ptr, int32 numBytes) {
    return 0;
}

//------------------------------------------------------------------------------
bool
dummyFSWrapper::seek(handle f, int32 offset) {
    return true;
}

//------------------------------------------------------------------------------
void
dummyFSWrapper::closeFile(handle f) {
    // empty
}

//------------------------------------------------------------------------------
String
dummyFSWrapper::getExecutablePath() {
    return String();
}

//------------------------------------------------------------------------------
String
dummyFSWrapper::getCwd() {
    return String();
}

} // namespace _priv
} // namespace Oryol

