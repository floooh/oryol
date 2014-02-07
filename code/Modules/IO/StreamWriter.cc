//------------------------------------------------------------------------------
//  StreamWriter.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "StreamWriter.h"
#include "Stream.h"

namespace Oryol {
namespace IO {

using namespace Oryol::Core;

OryolClassImpl(StreamWriter);

//------------------------------------------------------------------------------
StreamWriter::StreamWriter() {
    // empty
}

//------------------------------------------------------------------------------
StreamWriter::StreamWriter(const Ptr<Stream>& s) {
    o_assert(s->IsOpen());
    o_assert(s->IsWritable());
    this->stream = s;
}

//------------------------------------------------------------------------------
StreamWriter::~StreamWriter() {
    if (this->stream) {
        this->DetachStream();
    }
}

//------------------------------------------------------------------------------
void
StreamWriter::AttachStream(const Ptr<Stream>& s) {
    o_assert(s->IsOpen());
    o_assert(s->IsWritable());
    o_assert(s.isValid());
    if (this->stream) {
        this->DetachStream();
    }
    this->stream = s;
}

//------------------------------------------------------------------------------
void
StreamWriter::DetachStream() {
    if (this->stream) {
        this->stream = 0;
    }
}

//------------------------------------------------------------------------------
const Ptr<Stream>&
StreamWriter::GetStream() const {
    return this->stream;
}

} // namespace IO
} // namespace Oryol