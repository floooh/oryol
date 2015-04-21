#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::imguiWrapper
    @brief imgui wrapper class for Oryol
*/
#include "Core/Types.h"
#include "IMUI/IMUISetup.h"
#include "Input/Core/Mouse.h"
#include "Input/Core/Key.h"
#include "Input/InputProtocol.h"
#include "Messaging/Dispatcher.h"
#include "Gfx/Gfx.h"

namespace Oryol {
namespace _priv {

class imguiWrapper {
public:
    /// setup the wrapper
    void Setup(const IMUISetup& setup);
    /// discard the wrapper
    void Discard();
    /// return true if wrapper is valid
    bool IsValid() const;
    /// draw one frame
    void Draw();

private:
    /// setup dynamic mesh
    void setupMesh();
    /// setup font texture
    void setupFontTexture();
    /// setup draw state
    void setupDrawState();

    bool isValid = false;
    Ptr<Dispatcher<InputProtocol>> inputHandler;
    ResourceLabel resLabel;
    VertexLayout vertexLayout;
    Id fontTexture;
    Id mesh;
    Id drawState;
};

} // namespace _priv
} // namespace Oryol