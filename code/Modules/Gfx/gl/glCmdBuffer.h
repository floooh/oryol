#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glCmdBuffer
    @brief record and replay rendering commands
    
    The glCmdBuffer is useful to make efficient us of of uniform buffers.
    There is one big uniform buffer per frame which records all
    uniform updates for one frame, instead of many granular uniform buffers.
*/
#include "Core/Types.h"
#include "Gfx/Setup/GfxSetup.h"
#include "Gfx/Core/gfxPointers.h"
#include "Gfx/gl/gl_decl.h"
#include <cstring>

namespace Oryol {
namespace _priv {

class pipeline;
class mesh;
class texture;
class glRenderer;

class glCmdBuffer {
public:
    /// setup the cmd buffer object
    void setup(const GfxSetup& gfxSetup);
    /// discard the cmd buffer object
    void discard();
    /// set uniform buffer base pointer for current frame
    void setCurrentUniformBuffer(uint8_t* ptr, int size);
    /// flush the uniform- and command-buffer (play back recorded commands)
    void flush(glRenderer* renderer);

    /// apply render target
    void rendertarget(texture* rt, const ClearState& clearState);
    /// set viewport
    void viewport(int x, int y, int w, int h, bool originTopLeft);
    /// set scissor rect
    void scissor(int x, int y, int w, int h, bool originTopLeft);
    /// bind drawState
    void drawState(pipeline* pip, mesh** meshes, int numMeshes);
    /// bind textures
    void textures(ShaderStage::Code bindStage, texture** textures, int numTextures);
    /// bind uniform block
    void uniformBlock(ShaderStage::Code bindStage, int bindSlot, uint32_t layoutHash, const uint8_t* ptr, int byteSize);
    /// draw command
    void draw(int baseElement, int numElements);
    /// draw-instanced command
    void drawInstanced(int baseElement, int numElements, int numInstances);
    /// draw command with primGroupIndex
    void drawPrimGroupIndex(int primGroupIndex);
    /// draw-instanced with primGroupIndex
    void drawInstancedPrimGroupIndex(int primGroupIndex, int numInstances);

    /// check if cmd buffer has room for n elements
    bool cmdCheckRoom(int n) const {
        return (this->cmdCurIndex + n) < this->cmdEndIndex;
    }
    /// add command buffer element
    template<typename T> void cmdPut(T val) {
        o_assert_dbg(this->cmdCurIndex < this->cmdEndIndex);
        this->cmdBuffer[this->cmdCurIndex++] = (uintptr_t) val;
    }
    /// get command buffer element
    template<typename T> T cmdGet(int index) {
        o_assert_dbg(index < this->cmdCurIndex);
        return (T) this->cmdBuffer[index];
    }
    /// get typed pointer into cmd buffer
    template<typename T> T* cmdPtr(int index) {
        o_assert_dbg(index < this->cmdCurIndex);
        return (T*) &(this->cmdBuffer[index]);
    }

    /// check if uniform buffer has room
    bool ubCheckRoom(int n) const;
    /// copy to uniform buffer
    void ubCopy(const uint8_t* ptr, int byteSize) {
        o_assert_dbg((this->ubCurIndex+byteSize) < this->ubEndIndex);
        std::memcpy(&(this->uniformBuffer[this->ubCurIndex]), ptr, byteSize);
        this->ubCurIndex = Memory::RoundUp(this->ubCurIndex + byteSize, this->ubAlign);
    };

    enum cmd {
        cmdRenderTarget,
        cmdViewport,
        cmdScissor,
        cmdDrawState,
        cmdTextures,
        cmdUniformBlock,
        cmdDraw,
        cmdDrawInstanced,
        cmdDrawPrimGroupIndex,
        cmdDrawInstancedPrimGroupIndex,
    };

    bool isValid = false;

    int cmdEndIndex = 0;
    int cmdCurIndex = 0;
    uintptr_t* cmdBuffer = nullptr;

    int ubEndIndex = 0;
    int ubCurIndex = 0;
    int ubStartIndex = 0;
    int ubAlign = 0;
    uint8_t* uniformBuffer = nullptr;
};

} // namespace _priv
} // namespace Oryol
