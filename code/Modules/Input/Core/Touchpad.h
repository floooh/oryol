#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Input::Touchpad
    @brief access to touchpad state
*/
#include "Core/Types.h"

namespace Oryol {
namespace Input {
    
class Touchpad {
public:
    /// constructor
    Touchpad();
    
    /// return true if mouse is valid/attached
    bool Attached() const;
    
    /// FIXME: add actual API
    
    /// set touchpad attached state
    void setAttached(bool b);
    /// reset the touchpad state
    void reset();
    
private:
    bool attached;
};
    
} // namespace Input
} // namespace Oryol