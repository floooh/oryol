//------------------------------------------------------------------------------
//  TextureLoaderBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "TextureLoaderBase.h"

namespace Oryol {

OryolClassImpl(TextureLoaderBase);

//------------------------------------------------------------------------------
void
TextureLoaderBase::Prepare(const Id& id_, const TextureSetup& setup_) {
    o_assert_dbg(!this->id.IsValid());
    this->id = id_;
    this->setup = setup_;
}

} // namespace Oryol