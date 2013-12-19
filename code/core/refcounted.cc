//------------------------------------------------------------------------------
//  refcounted.cc
//------------------------------------------------------------------------------
#include "pre.h"
#include "refcounted.h"

namespace oryol {
namespace core {

oryol_class_impl(refcounted, 4096);

//------------------------------------------------------------------------------
refcounted::~refcounted() {
    // empty
}

} // namespace oryol
} // namespace core