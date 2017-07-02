#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::winInputMgr
    @ingroup _priv
    @brief provide input for the Windows D3D-based ports

    Input handling for the D3D ports is similar to GLFW (on purpose), most
    of the window and input handling code has been copied from GLFW to ensure
    same behaviour between the GL and D3D versions of Oryol.
*/
#include "Input/private/inputMgrBase.h"

namespace Oryol {
namespace _priv {

class winInputMgr : public inputMgrBase {
public:
    /// constructor
    winInputMgr();
    /// destructor
    ~winInputMgr();

    /// setup the window input manager
    void setup(const InputSetup& setup);
    /// discard the windows input manager
    void discard();

    /// setup the key mapping table
    void setupKeyTable();
    /// setup input callbacks
    void setupCallbacks();
    /// discard input callbacks
    void discardCallbacks();
    /// map winDisplayMgr keycode to Oryol keycode
    Key::Code mapKey(int key) const;

    /// key callback
    static void keyCallback(int key, int scancode, int action, int mods);
    /// char callback
    static void charCallback(unsigned int unicode);
    /// mouse button callback
    static void mouseButtonCallback(int button, int action, int mods);
    /// cursor position callback
    static void cursorPosCallback(double x, double y);
    /// cursor enter callback
    static void cursorEnterCallback(int entered);
    /// scroll callback
    static void scrollCallback(double xOffset, double yOffset);

    static winInputMgr* self;
    int runLoopId;
};

} // namespace _priv
} // namespace Oryol