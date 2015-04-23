#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::imguiWrapper
    @brief imgui wrapper class for Oryol
*/
#include "Core/Types.h"
#include "Input/Core/Mouse.h"
#include "Input/Core/Key.h"
#include "Input/InputProtocol.h"
#include "Messaging/Dispatcher.h"
#include "Gfx/Gfx.h"
#include "Time/Duration.h"
#include "imgui.h"

namespace Oryol {
namespace _priv {

class imguiWrapper {
public:
    /// setup the wrapper
    void Setup();
    /// discard the wrapper
    void Discard();
    /// return true if wrapper is valid
    bool IsValid() const;
    /// call before issuing ImGui commands
    void NewFrame(Duration frameDuration);

private:
    /// setup dynamic mesh
    void setupMesh();
    /// setup font texture
    void setupFontTexture();
    /// setup draw state
    void setupDrawState();
    /// imgui's draw callback
    static void imguiRenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count);

    static const int MaxNumVertices = 64 * 1024;    

    static imguiWrapper* self;

    bool isValid = false;
    ResourceLabel resLabel;
    Id fontTexture;
    Id mesh;
    Id drawState;
    ImDrawVert vertexData[MaxNumVertices];
};

} // namespace _priv
} // namespace Oryol