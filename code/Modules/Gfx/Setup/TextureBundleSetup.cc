//------------------------------------------------------------------------------
//  TextureBundleSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "TextureBundleSetup.h"

namespace Oryol {

//------------------------------------------------------------------------------
TextureBundleSetup::TextureBundleSetup() :
Locator(Locator::NonShared()) {
    // empty
}

//------------------------------------------------------------------------------
TextureBundleSetup::TextureBundleSetup(const class Locator& loc) :
Locator(loc) {
    // empty
}

} // namespace Oryol
