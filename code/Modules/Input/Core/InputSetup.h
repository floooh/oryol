#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::InputSetup
    @ingroup Input
    @brief configure the Input module
*/
#include "Core/Types.h"
#include "Core/Containers/Map.h"
#include "Core/String/StringAtom.h"
#include "Input/Core/GamepadMapping.h"

namespace Oryol {

class InputSetup {
public:
    /// initial set of gamepad mappings, use id="__default" to override default mapping
    Map<StringAtom, GamepadMapping> GamepadMappings;
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
};
    
} // namespace Oryol
