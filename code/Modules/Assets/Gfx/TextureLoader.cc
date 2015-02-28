//------------------------------------------------------------------------------
//  TextureLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "TextureLoader.h"
#include "Gfx/Gfx.h"
#include "IO/Stream/Stream.h"
#define GLIML_ASSERT(x) o_assert(x)
#include "gliml.h"

namespace Oryol {

OryolClassImpl(TextureLoader);

//------------------------------------------------------------------------------
void
TextureLoader::Loaded(const URL& url, int32 ioLane, const void* data, int32 numBytes) {
    // NOTE: this is running on a thread!
    o_assert(this->id.IsValid());
    
    // parse the data with gliml, just accept all texture formats,
    // GL will complain later if this is not supported
    gliml::context ctx;
    ctx.enable_dxt(true);
    ctx.enable_pvrtc(true);
    ctx.enable_etc2(true);
    if (ctx.load(data, numBytes)) {
        TextureSetup texSetup = this->buildSetup(this->setup, &ctx);
        Ptr<Stream> stream = this->buildStream(data, numBytes);
        SetupAndStream<TextureSetup> setupAndStream(texSetup, stream);
        Gfx::Resource().initResourceFromThread(ioLane, this->id, setupAndStream);
    }
}

//------------------------------------------------------------------------------
void
TextureLoader::Failed(const URL& url, int32 ioLane, IOStatus::Code ioStatus) {
    Gfx::Resource().failResourceFromThread(ioLane, this->id);
}

//------------------------------------------------------------------------------
TextureSetup
TextureLoader::buildSetup(const TextureSetup& blueprint, const gliml::context* ctx) {
    o_error("FIXME FIXME FIXME");
    return blueprint;
}

//------------------------------------------------------------------------------
Ptr<Stream>
TextureLoader::buildStream(const void* data, int32 numBytes) {
    o_assert_dbg(nullptr != data);
    o_assert_dbg(0 < numBytes);
    Ptr<MemoryStream> stream = MemoryStream::Create();
    stream->Reserve(numBytes);
    stream->Open(OpenMode::WriteOnly);
    stream->Write(data, numBytes);
    stream->Close();
    return stream;
}

} // namespace Oryol