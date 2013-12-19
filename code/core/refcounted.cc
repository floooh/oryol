//------------------------------------------------------------------------------
//  refcounted.cc
//------------------------------------------------------------------------------
#include "pre.h"
#include "refcounted.h"

namespace oryol {
namespace core {

oryol_class_impl(refcounted, 1024);

//------------------------------------------------------------------------------
refcounted::~refcounted() {
    // empty
}

} // namespace oryol
} // namespace core