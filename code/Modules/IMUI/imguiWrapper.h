#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::imguiWrapper
    @brief imgui wrapper class for Oryol
*/
#include "Core/Types.h"
#include "Input/Core/Mouse.h"
#include "Input/Core/Key.h"
#include "Gfx/Gfx.h"
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
    void NewFrame(float frameDurationInSeconds);

private:
    /// setup font texture
    void setupFontTexture();
    /// setup draw state
    void setupMeshAndDrawState();
    /// imgui's draw callback
    static void imguiRenderDrawLists(ImDrawData* draw_data);

    static const int MaxNumVertices = 64 * 1024;
    static const int MaxNumIndices = 128 * 1024;

    static imguiWrapper* self;

    bool isValid = false;
    ResourceLabel resLabel;
    DrawState drawState;
    ImDrawVert vertexData[MaxNumVertices];
    ImDrawIdx indexData[MaxNumIndices];
};

} // namespace _priv
} // namespace Oryol