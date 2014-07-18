#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Input::Button
    @brief input button codes
*/
#include "Core/Types.h"

namespace Oryol {
namespace Input {
    
class Button {
public:
    enum Code {
        InvalidButton = 0,
        
        Left,       ///< left mouse button
        Middle,     ///< middle mouse button
        Right,      ///< right mouse button
        ZoomIn,     ///< mouse wheel forward
        ZoomOut,    ///< mouse wheel backward
        
        A,          ///< game pad A button
        B,          ///< game pad B button
        X,          ///< game pad X button
        Y,          ///< game pad Y button
        
        NumButtons
    };
};

    
} // namespace Input
} // namespace Oryol