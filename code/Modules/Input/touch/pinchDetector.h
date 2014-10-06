#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::pinchDetector
    @brief 2-finger-pinch gesture detector
*/
#include "Input/touch/touchEvent.h"
#include "Input/touch/gestureState.h"
#include "glm/vec2.hpp"

namespace Oryol {
namespace _priv {
    
class pinchDetector {
public:
    /// feed new touch event and return detected state
    gestureState::Code detect(const touchEvent& newEvent);
    /// reset the detector
    void reset();

    /// get position of first touch
    glm::vec2 position0;
    /// get position of second touch
    glm::vec2 position1;
    /// get start position of first touch
    glm::vec2 startPosition0;
    /// get start position of second touch
    glm::vec2 startPosition1;
    
private:
    touchEvent startEvent;
};
    
} // namespace _priv
} // namespace Oryol