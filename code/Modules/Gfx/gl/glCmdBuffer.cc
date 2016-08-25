//------------------------------------------------------------------------------
//  glCmdBuffer.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glCmdBuffer.h"
#include "glRenderer.h"
#include "Core/Memory/Memory.h"
#include "Gfx/Core/DrawState.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
void
glCmdBuffer::setup(const GfxSetup& gfxSetup) {
    o_assert_dbg(!this->isValid);
    o_assert_dbg(!this->cmdBuffer && !this->uniformBuffer);
    this->isValid = true;

    // compute size of cmd buffer and allocate
    this->cmdCurIndex = 0;
    this->cmdEndIndex = ((gfxSetup.MaxApplyDrawStatesPerFrame * sizeof(DrawState)) / sizeof(uintptr_t)) +
                        gfxSetup.MaxDrawCallsPerFrame * 6;
    this->cmdBuffer = (uintptr_t*) Memory::Alloc(this->cmdEndIndex * sizeof(uintptr_t));

    // allocate the global uniform buffer
    this->ubCurIndex = 0;
    this->ubEndIndex = gfxSetup.GlobalUniformBufferSize;
    this->uniformBuffer = (uint8_t*) Memory::Alloc(this->ubEndIndex);
}

//------------------------------------------------------------------------------
void
glCmdBuffer::discard() {
    o_assert_dbg(this->isValid);
    o_assert_dbg(this->cmdBuffer && this->uniformBuffer);
    Memory::Free(this->cmdBuffer);
    this->cmdBuffer = nullptr;
    Memory::Free(this->uniformBuffer);
    this->uniformBuffer = nullptr;
    this->isValid = false;
}

//------------------------------------------------------------------------------
void
glCmdBuffer::viewport(int x, int y, int w, int h, bool originTopLeft) {
    o_assert_dbg(this->isValid);
    if (this->cmdCheckRoom(6)) {
        this->cmdPut(cmdViewport);
        this->cmdPut(x);
        this->cmdPut(y);
        this->cmdPut(w);
        this->cmdPut(h);
        this->cmdPut(originTopLeft);
    }
}

//------------------------------------------------------------------------------
void
glCmdBuffer::scissor(int x, int y, int w, int h, bool originTopLeft) {
    o_assert_dbg(this->isValid);
    if (this->cmdCheckRoom(5)) {
        this->cmdPut(cmdScissor);
        this->cmdPut(x);
        this->cmdPut(y);
        this->cmdPut(w);
        this->cmdPut(h);
        this->cmdPut(originTopLeft);
    }
}

//------------------------------------------------------------------------------
void
glCmdBuffer::drawState(pipeline* pip, mesh** meshes, int numMeshes) {
    o_assert_dbg(this->isValid);
    if (this->cmdCheckRoom(3 + numMeshes)) {
        this->cmdPut(cmdDrawState);
        this->cmdPut(pip);
        this->cmdPut(numMeshes);
        for (int i = 0; i < numMeshes; i++) {
            this->cmdPut(meshes[i]);
        }
    }
}

//------------------------------------------------------------------------------
void
glCmdBuffer::textures(ShaderStage::Code bindStage, texture** textures, int numTextures) {
    o_assert_dbg(this->isValid);
    if (this->cmdCheckRoom(3 + numTextures)) {
        this->cmdPut(cmdTextures);
        this->cmdPut(bindStage);
        this->cmdPut(numTextures);
        for (int i = 0; i < numTextures; i++) {
            this->cmdPut(textures[i]);
        }
    }
}

//------------------------------------------------------------------------------
void
glCmdBuffer::uniformBlock(ShaderStage::Code bindStage, int bindSlot, uint32_t layoutHash, const uint8_t* ptr, int byteSize) {
    o_assert_dbg(this->isValid);
    if (this->cmdCheckRoom(6) && this->ubCheckRoom(byteSize)) {
        this->cmdPut(cmdUniformBlock);
        this->cmdPut(bindStage);
        this->cmdPut(bindSlot);
        this->cmdPut(layoutHash);
        this->cmdPut(this->ubCurIndex);
        this->cmdPut(byteSize);
        this->ubCopy(ptr, byteSize);
    }
}

//------------------------------------------------------------------------------
void
glCmdBuffer::draw(int baseElement, int numElements) {
    o_assert_dbg(this->isValid);
    if (this->cmdCheckRoom(3)) {
        this->cmdPut(cmdDraw);
        this->cmdPut(baseElement);
        this->cmdPut(numElements);
    }
}

//------------------------------------------------------------------------------
void
glCmdBuffer::drawInstanced(int baseElement, int numElements, int numInstances) {
    o_assert_dbg(this->isValid);
    if (this->cmdCheckRoom(4)) {
        this->cmdPut(cmdDrawInstanced);
        this->cmdPut(baseElement);
        this->cmdPut(numElements);
        this->cmdPut(numInstances);
    }
}

//------------------------------------------------------------------------------
void
glCmdBuffer::drawPrimGroupIndex(int primGroupIndex) {
    o_assert_dbg(this->isValid);
    if (this->cmdCheckRoom(2)) {
        this->cmdPut(cmdDrawPrimGroupIndex);
        this->cmdPut(primGroupIndex);
    }
}

//------------------------------------------------------------------------------
void
glCmdBuffer::drawInstancedPrimGroupIndex(int primGroupIndex, int numInstances) {
    o_assert_dbg(this->isValid);
    if (this->cmdCheckRoom(3)) {
        this->cmdPut(cmdDrawInstancedPrimGroupIndex);
        this->cmdPut(primGroupIndex);
        this->cmdPut(numInstances);
    }
}

//------------------------------------------------------------------------------
void
glCmdBuffer::flush(glRenderer* r) {
    o_assert_dbg(this->isValid);
    int i = 0;
    while (i < this->cmdCurIndex) {
        switch (this->cmdGet<cmd>(i++)) {
            case cmdViewport:
                {
                    int x = this->cmdGet<int>(i++);
                    int y = this->cmdGet<int>(i++);
                    int w = this->cmdGet<int>(i++);
                    int h = this->cmdGet<int>(i++);
                    bool originTopLeft = this->cmdGet<bool>(i++);
                    r->applyViewPort(x, y, w, h, originTopLeft, false);
                }
                break;
            case cmdScissor:
                {
                    int x = this->cmdGet<int>(i++);
                    int y = this->cmdGet<int>(i++);
                    int w = this->cmdGet<int>(i++);
                    int h = this->cmdGet<int>(i++);
                    bool originTopLeft = this->cmdGet<bool>(i++);
                    r->applyScissorRect(x, y, w, h, originTopLeft, false);
                }
                break;
            case cmdDrawState:
                {
                    pipeline* pip = this->cmdGet<pipeline*>(i++);
                    int numMeshes = this->cmdGet<int>(i++);
                    mesh** meshes = this->cmdPtr<mesh*>(i);
                    r->applyDrawState(pip, meshes, numMeshes, false);
                    i += numMeshes;
                }
                break;

            case cmdTextures:
                {
                    ShaderStage::Code bindStage = this->cmdGet<ShaderStage::Code>(i++);
                    int numTextures = this->cmdGet<int>(i++);
                    texture** textures = this->cmdPtr<texture*>(i);
                    r->applyTextures(bindStage, textures, numTextures, false);
                    i += numTextures;
                }
                break;

            case cmdUniformBlock:
                {
                    ShaderStage::Code bindStage = this->cmdGet<ShaderStage::Code>(i++);
                    int bindSlot = this->cmdGet<int>(i++);
                    uint32_t layoutHash = this->cmdGet<int64_t>(i++);
                    int offset = this->cmdGet<int>(i++);
                    int byteSize = this->cmdGet<int>(i++);
                    const uint8_t* ptr = &(this->uniformBuffer[offset]);
                    r->applyUniformBlock(bindStage, bindSlot, layoutHash, ptr, byteSize, false);
                }
                break;

            case cmdDraw:
                {
                    int baseElement = this->cmdGet<int>(i++);
                    int numElements = this->cmdGet<int>(i++);
                    r->draw(PrimitiveGroup(baseElement, numElements), false);
                }
                break;

            case cmdDrawInstanced:
                {
                    int baseElement = this->cmdGet<int>(i++);
                    int numElements = this->cmdGet<int>(i++);
                    int numInstances = this->cmdGet<int>(i++);
                    r->drawInstanced(PrimitiveGroup(baseElement, numElements), numInstances, false);
                }
                break;

            case cmdDrawPrimGroupIndex:
                {
                    int primGroupIndex = this->cmdGet<int>(i++);
                    r->draw(primGroupIndex, false);
                }
                break;

            case cmdDrawInstancedPrimGroupIndex:
                {
                    int primGroupIndex = this->cmdGet<int>(i++);
                    int numInstances = this->cmdGet<int>(i++);
                    r->drawInstanced(primGroupIndex, numInstances, false);
                }
                break;
        }
    }
    this->cmdCurIndex = 0;
    this->ubCurIndex = 0;
}

} // namespace _priv
} // namespace Oryol
