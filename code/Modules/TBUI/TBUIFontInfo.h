#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::TBUIFontInfo
    @brief holds path and name of a font
*/
#include "Core/String/StringAtom.h"
#include "IO/Core/URL.h"

namespace Oryol {

class TBUIFontInfo {
public:
    /// constructor
    TBUIFontInfo(const URL& loc, const StringAtom& name);
    
    URL Location;
    StringAtom Name;
};

//------------------------------------------------------------------------------
inline
TBUIFontInfo::TBUIFontInfo(const URL& loc, const StringAtom& name) :
Location(loc),
Name(name) {
    // empty
}

} // namespace Oryol