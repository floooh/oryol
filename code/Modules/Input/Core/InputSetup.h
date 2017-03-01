#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::InputSetup
    @ingroup Input
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
    /// accelerometer enabled
    bool AccelerometerEnabled = true;
    /// gyrometer enabled
    bool GyrometerEnabled = true;
    /// gamepad polling
    bool PollingRequired = false;
};
    
} // namespace Oryol
