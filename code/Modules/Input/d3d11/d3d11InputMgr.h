#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11InputMgr
    @ingroup _priv
    @brief provide input for the D3D11 port

    Input handling for the D3D11 port is similar to GLFW (on purpose), most
    of the window and input handling code has been copied from GLFW to ensure
    same behaviour between the GL and D3D11 versions of Oryol.
*/
#include "Input/base/inputMgrBase.h"

namespace Oryol {
namespace _priv {

class d3d11InputMgr : public inputMgrBase {
public:
    /// constructor
    d3d11InputMgr();
    /// destructor
    ~d3d11InputMgr();

    /// setup the d3d11 input manager
    void setup(const InputSetup& setup);
    /// discard the d3d11 input manager
    void discard();

    /// set mouse cursor mode
    void setCursorMode(CursorMode::Code mode);

private:
    /// setup the key mapping table
    void setupKeyTable();
    /// setup input callbacks
    void setupCallbacks();
    /// discard input callbacks
    void discardCallbacks();
    /// map d3d11DisplayMgr keycode to Oryol keycode
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

    static d3d11InputMgr* self;
    int32 runLoopId;
};

} // namespace _priv
} // namespace Oryol