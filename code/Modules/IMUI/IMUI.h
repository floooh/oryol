#pragma once
//------------------------------------------------------------------------------
/**
    @defgroup IMGUI IMGUI
    @brief Ocornut's ImGUI Wrapper Module

    @class Oryol::IMUI
    @ingroup IMUI
    @brief facade of the ImGUI module
    
    Why IMUI not IMGUI? Because of filename collisions.
*/
#include "Core/Types.h"
#include "IMUI/imguiWrapper.h"
#include "Core/Time/Duration.h"

namespace Oryol {

class IMUI {
public:
    /// setup the IMUI module
    static void Setup();
    /// shutdown the IMUI module
    static void Discard();
    /// test if IMUI module has been setup
    static bool IsValid();
    /// start new ImGui frame, with frame time
    static void NewFrame(Duration frameDuration);
    /// start new ImGui frame, with fixed 1/60sec frametime
    static void NewFrame();

private:
    struct _state {
        _priv::imguiWrapper imguiWrapper;
    };
    static _state* state;
};

//------------------------------------------------------------------------------
inline bool
IMUI::IsValid() {
    return nullptr != state;
}

} // namespace Oryol