#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::osxInputMgr
    @ingroup _priv
    @brief input manager wrapper for OSX without GLFW
*/
#include "Input/base/inputMgrBase.h"

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

private:
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
    static void charCallback(uint32 unicode);
    /// GLFW mouse button callback
    static void mouseButtonCallback(int button, int action, int mods);
    /// GLFW cursor position callback
    static void cursorPosCallback(float64 x, float64 y);
    /// GLFW cursor enter callback
    static void cursorEnterCallback(bool entered);
    /// GLFW scroll callback
    static void scrollCallback(float64 xOffset, float64 yOffset);
    
    static osxInputMgr* self;
    int32 runLoopId;
};

} // namespace _priv
} // namespace Oryol

