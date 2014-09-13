#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::CursorMode
    @ingroup Input
    @brief mouse cursor modes
*/
namespace Oryol {

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
    
} // namespace Oryol