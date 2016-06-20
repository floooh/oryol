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
int
dummyFSWrapper::write(handle f, const void* ptr, int numBytes) {
    return 0;
}

//------------------------------------------------------------------------------
int
dummyFSWrapper::read(handle f, void* ptr, int numBytes) {
    return 0;
}

//------------------------------------------------------------------------------
bool
dummyFSWrapper::seek(handle f, int offset) {
    return true;
}

//------------------------------------------------------------------------------
int
dummyFSWrapper::size(handle f) {
    return 0;
}

//------------------------------------------------------------------------------
void
dummyFSWrapper::close(handle f) {
    // empty
}

//------------------------------------------------------------------------------
String
dummyFSWrapper::getExecutableDir() {
    return String();
}

//------------------------------------------------------------------------------
String
dummyFSWrapper::getCwd() {
    return String();
}

} // namespace _priv
} // namespace Oryol

