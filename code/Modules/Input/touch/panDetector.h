#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::panDetector
    @brief panning gesture detector
*/
#include "Input/touch/touchEvent.h"
#include "Input/touch/gestureState.h"
#include "glm/vec2.hpp"

namespace Oryol {
namespace _priv {
    
class panDetector {
public:
    /// minimum distance after touch is detected as panning
    static const int32 MinMovedDistance = 10;
    
    /// feed new touch event and return detected state
    gestureState::Code detect(const touchEvent& newEvent);
    /// reset the detector
    void reset();
    /// get current position
    glm::vec2 position;
    /// get pan start position
    glm::vec2 startPosition;
    
private:
    /// check if distance between 2 points is less then
    bool distLess(const glm::vec2& pos0, const glm::vec2& pos1, float dist) const;
    
    touchEvent startEvent;
    bool panning = false;
};
    
} // namespace _priv
} // namespace Oryol
