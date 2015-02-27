//------------------------------------------------------------------------------
//  MeshLoaderBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "MeshLoaderBase.h"

namespace Oryol {

OryolClassImpl(MeshLoaderBase);

//------------------------------------------------------------------------------
void
MeshLoaderBase::Prepare(const Id& id_, const MeshSetup& setup_) {
    o_assert_dbg(!this->id.IsValid());
    this->id = id_;
    this->setup = setup_;
}

} // namespace Oryol