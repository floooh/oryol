//------------------------------------------------------------------------------
//  ResourceLoader.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ResourceLoader.h"

namespace Oryol {

OryolClassImpl(ResourceLoader);

//------------------------------------------------------------------------------
ResourceLoader::ResourceLoader() :
isAttached(false) {
    // empty
}

//------------------------------------------------------------------------------
ResourceLoader::~ResourceLoader() {
    o_assert_dbg(!this->isAttached);
}

//------------------------------------------------------------------------------
void
ResourceLoader::OnAttach() {
    o_assert_dbg(!this->isAttached);
    this->isAttached = true;
}

//------------------------------------------------------------------------------
void
ResourceLoader::OnDetach() {
    o_assert_dbg(this->isAttached);
    this->isAttached = false;
}

//------------------------------------------------------------------------------
bool
ResourceLoader::IsAttached() const {
    return this->isAttached;
}

} // namespace Oryol