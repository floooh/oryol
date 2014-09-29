#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::touch
    @brief internal touch event
*/
#include "Core/Types.h"
#include "Time/TimePoint.h"

namespace Oryol {
namespace _priv {
    
class touch {
public:
    /// touch event type
    enum touchType {
        began,
        moved,
        ended,
        cancelled,
        invalid,
    } type = invalid;
    /// time stamp when the event occured
    TimePoint time;
    /// number of touch points in the event
    int32 numTouches = 0;
    /// max number of touches
    static const int32 MaxNumPoints = 8;
    /// touch points
    struct point {
        uintptr identifier = 0;     // unique 'finger identifier'
        float32 x = 0.0f;
        float32 y = 0.0f;
        bool isChanged = false;
    } points[MaxNumPoints];
};
    
} // namespace _priv
} // namespace Oryol