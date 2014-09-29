#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::tapDetector
    @brief simple tap-gesture recognizer
*/
#include "Input/touch/touch.h"
#include "Input/touch/gestureState.h"
#include "glm/vec2.hpp"

namespace Oryol {
namespace _priv {
    
class tapDetector {
public:
    /// timeout for taps in milliseconds
    static const int32 Timeout = 250;
    /// max pixel distance from tap start
    static const int32 MaxDistance = 10;

    /// feed new touch event and return detected state (if any)
    gestureState::Code detect(const touch& touchEvent);
    /// get current tap position
    const glm::vec2& pos() const;
    
private:
    touch startEvent;
    glm::vec2 position;
};
    
} // namespace _priv
} // namespace Oryol