#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Input::CursorMode
    @brief mouse cursor modes
*/
namespace Oryol {
namespace Input {

class CursorMode {
public:
    enum Code {
        Normal = 0, ///< cursor visible, mouse pos clamped by screen border
        Hidden,     ///< cursor invisible, mouse pos clamped by screen border
        Disabled,   ///< cursor invisible, unbound movement
        
        NumCursorModes,
        InvalidCursorMode
    };
};
    
} // namespace Input
} // namespace Oryol