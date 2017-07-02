#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::osxInputMgr
    @ingroup _priv
    @brief input manager wrapper for OSX without GLFW
*/
#include "Input/private/inputMgrBase.h"

namespace Oryol {
namespace _priv {

class osxInputMgr : public inputMgrBase {
public:
    /// constructor
    osxInputMgr();
    /// destructor
    ~osxInputMgr();

    /// setup the input manager
    void setup(const InputSetup& setup);
    /// discard the input manager
    void discard();

    /// setup the key mapping table
    void setupKeyTable();
    /// setup glfw input callbacks
    void setupCallbacks();
    /// discard glfw input callbacks
    void discardCallbacks();
    /// map GLFW key code to Oryol key code
    Key::Code mapKey(int osxBridgeKey) const;
    
    /// GLFW key callback
    static void keyCallback(int key, int scancode, int action, int mods);
    /// GLFW char callback
    static void charCallback(uint32_t unicode);
    /// GLFW mouse button callback
    static void mouseButtonCallback(int button, int action, int mods);
    /// GLFW cursor position callback
    static void cursorPosCallback(double x, double y);
    /// GLFW cursor enter callback
    static void cursorEnterCallback(bool entered);
    /// GLFW scroll callback
    static void scrollCallback(double xOffset, double yOffset);
    
    static osxInputMgr* self;
    int runLoopId;
};

} // namespace _priv
} // namespace Oryol

