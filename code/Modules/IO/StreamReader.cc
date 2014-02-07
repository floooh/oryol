//------------------------------------------------------------------------------
//  StreamReader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "StreamReader.h"
#include "Stream.h"

namespace Oryol {
namespace IO {

using namespace Oryol::Core;

OryolClassImpl(StreamReader);

//------------------------------------------------------------------------------
StreamReader::StreamReader() {
    // empty
}

//------------------------------------------------------------------------------
StreamReader::StreamReader(const Ptr<Stream>& s) {
    o_assert(s->IsOpen());
    o_assert(s->IsReadable());
    this->stream = s;
}

//------------------------------------------------------------------------------
StreamReader::~StreamReader() {
    if (this->stream) {
        this->DetachStream();
    }
}

//------------------------------------------------------------------------------
void
StreamReader::AttachStream(const Ptr<Stream>& s) {
    o_assert(s->IsOpen());
    o_assert(s->IsReadable());
    o_assert(s.isValid());
    if (this->stream) {
        this->DetachStream();
    }
    this->stream = s;
}

//------------------------------------------------------------------------------
void
StreamReader::DetachStream() {
    if (this->stream) {
        this->stream = 0;
    }
}

//------------------------------------------------------------------------------
const Ptr<Stream>&
StreamReader::GetStream() const {
    return this->stream;
}

} // namespace IO
} // namespace Oryol