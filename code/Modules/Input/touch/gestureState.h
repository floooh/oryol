#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::gestureState
    @ingroup _priv
    @brief result of a gesture detector
*/

namespace Oryol {
namespace _priv {

class gestureState {
public:
    /// gesture action codes
    enum Code {
        none  = 0,
        start = (1<<0),
        move  = (1<<1),
        end   = (1<<2),
        action = start|end,
    };
};
    
} // namespace _priv
} // namespace Oryol