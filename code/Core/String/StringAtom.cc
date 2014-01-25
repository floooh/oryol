//------------------------------------------------------------------------------
//  StringAtom.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "StringAtom.h"
#include "String.h"

namespace Oryol {
namespace Core {
const char* StringAtom::emptyString = "";

//------------------------------------------------------------------------------
StringAtom::StringAtom(const String& rhs) {
    this->setupFromCString(rhs.AsCStr());
}

//------------------------------------------------------------------------------
void
StringAtom::operator=(const String& rhs) {
    this->Clear();
    this->setupFromCString(rhs.AsCStr());
}

//------------------------------------------------------------------------------
String
StringAtom::AsString() const {
    if (0 != this->data) {
        return String(this->data->str);
    }
    else {
        return String();
    }
}

} // namespace Core
} // namespace Oryol