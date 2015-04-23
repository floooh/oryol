//------------------------------------------------------------------------------
/**
    @class Oryol::IMUI
    @brief facade of the ImGUI module
    
    Why IMUI not IMGUI? Because of filename collisions.
*/
#include "Core/Types.h"
#include "IMUI/imguiWrapper.h"

namespace Oryol {

class IMUI {
public:
    /// setup the IMUI module
    static void Setup();
    /// shutdown the IMUI module
    static void Discard();
    /// test if IMUI module has been setup
    static bool IsValid();
    /// call this before issuing ImGui commands
    static void NewFrame(Duration frameDuration);

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