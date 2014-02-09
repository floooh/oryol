//------------------------------------------------------------------------------
//  RefCounted.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "RefCounted.h"

namespace Oryol {
namespace Core {

OryolClassImpl(RefCounted);

//------------------------------------------------------------------------------
RefCounted::~RefCounted() {
    // empty
}

} // namespace Oryol
} // namespace Core