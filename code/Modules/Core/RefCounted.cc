//------------------------------------------------------------------------------
//  RefCounted.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "RefCounted.h"

namespace Oryol {

OryolClassImpl(RefCounted);

//------------------------------------------------------------------------------
RefCounted::~RefCounted() {
    // empty
}

} // namespace Core