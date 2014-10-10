#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Accelerometer
    @brief returns device acceleration data
*/
#include "glm/vec3.hpp"

namespace Oryol {

class Accelerometer {
public:
    /// does the host platform provide acceleration data?
    bool Attached = false;
    /// current acceleration vector, including gravity
    glm::vec3 AccelerationWithGravity;
    /// current acceleration with gravity removed
    glm::vec3 Acceleration;
};

} // namespace Oryol
