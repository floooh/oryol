#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::DbgSetup
    @brief setup parameters for the Dbg module
*/
#include "Core/Types.h"

namespace Oryol {

class DbgSetup {
public:
    /// number of text columns on whole screen
    int NumTextColumns = 120;
    /// number of text lines on whole screen
    int NumTextRows = 80;
    /// initial text scale, x dimension
    float TextScaleX = 1.0f;
    /// initial text scale, y dimension
    float TextScaleY = 1.0f;
};

} // namespace Oryol
