#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Touchpad
    @ingroup Input
    @brief access to touchpad state
*/
#include "Core/Types.h"
#include "glm/vec2.hpp"

namespace Oryol {
    
class Touchpad {
public:
    /// constructor
    Touchpad();
    
    /// touchpad valid?
    bool Attached;
    
    /// single-tapped?
    bool Tapped;
    /// double-tapped?
    bool DoubleTapped;
    /// pinching gesture underway?
    bool Pinching;
    /// panning gesture underway?
    bool Panning;

    /// current position
    glm::vec2 Position;
    /// current movement
    glm::vec2 Movement;

    /// reset the touchpad state
    void reset();
};

} // namespace Oryol
