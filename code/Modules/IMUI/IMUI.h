//------------------------------------------------------------------------------
/**
    @class Oryol::IMUI
    @brief facade of the ImGUI module
    
    Why IMUI not IMGUI? Because of filename collisions.
*/
#include "Core/Types.h"
#include "IMUI/IMUISetup.h"

namespace Oryol {

class IMUI {
public:
    /// setup the IMUI module
    static void Setup(const IMUISetup& setup);
    /// shutdown the IMUI module
    static void Discard();
    /// test if IMUI module has been setup
    static bool IsValid();

    /// draw one frame, call before Gfx::Commit()
    static void Draw();

private:
    struct _state {
        IMUISetup setup;
    };
    static _state* state;
};

//------------------------------------------------------------------------------
inline bool
IMUI::IsValid() {
    return nullptr != state;
}

} // namespace Oryol