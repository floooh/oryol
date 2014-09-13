#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Touchpad
    @ingroup Input
    @brief access to touchpad state
*/
#include "Core/Types.h"

namespace Oryol {
    
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
    
} // namespace Oryol