//------------------------------------------------------------------------------
//  RefCounted.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "RefCounted.h"

namespace Oryol {
namespace Core {

OryolClassImpl(RefCounted, 4096);

//------------------------------------------------------------------------------
RefCounted::~RefCounted() {
    // empty
}

} // namespace Oryol
} // namespace Core