#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::InputSetup
    @brief configure the Input module
*/
#include "Core/Types.h"

namespace Oryol {

class InputSetup {
public:
    /// enable/disable single-tap touch gesture
    bool TapEnabled = true;
    /// enable/disable double-tap touch gesture
    bool DoubleTapEnabled = true;
    /// enable/disable pinch touch gesture
    bool PinchEnabled = true;
    /// enable/disable pan touch gesture
    bool PanEnabled = true;
};
    
} // namespace Oryol
