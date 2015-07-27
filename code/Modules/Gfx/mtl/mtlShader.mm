//------------------------------------------------------------------------------
//  mtlShader.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlShader.h"
#include "mtl_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mtlShader::mtlShader() {
    this->Clear();
}

//------------------------------------------------------------------------------
mtlShader::~mtlShader() {
#if ORYOL_DEBUG
    for (int32 i = 0; i < this->numPrograms; i++) {
        o_assert_dbg(nil == this->programEntries[i].mtlVertexShader);
        o_assert_dbg(nil == this->programEntries[i].mtlFragmentShader);
    }
#endif
}

//------------------------------------------------------------------------------
void
mtlShader::Clear() {
    this->selMask = 0xFFFFFFFF;
    this->selIndex = 0;
    this->numPrograms = 0;
    this->programEntries.Fill(programEntry());
    this->mtlLibrary = nil;
    this->numUniformBlockEntries = 0;
    this->uniformBlockEntries.Fill(ubEntry());
    shaderBase::Clear();
}

//------------------------------------------------------------------------------
void
mtlShader::setLibrary(id<MTLLibrary> lib) {
    o_assert_dbg(nil != lib);
    this->mtlLibrary = lib;
}

//------------------------------------------------------------------------------
id<MTLLibrary>
mtlShader::getLibrary() const {
    return this->mtlLibrary;
}

//------------------------------------------------------------------------------
int32
mtlShader::addShaders(uint32 mask, id<MTLFunction> vsFunc, id<MTLFunction> fsFunc) {
    o_assert_dbg(this->numPrograms < GfxConfig::MaxNumBundlePrograms);
    o_assert_dbg(nil != vsFunc);
    o_assert_dbg(nil != fsFunc);

    // make sure the mask is unique
    for (int32 i = 0; i < this->numPrograms; i++) {
        o_assert_dbg(this->programEntries[i].mask != mask);
    }

    auto& entry = this->programEntries[this->numPrograms];
    entry.mask = mask;
    entry.mtlVertexShader = vsFunc;
    entry.mtlFragmentShader = fsFunc;

    return this->numPrograms++;
}

//------------------------------------------------------------------------------
uint32
mtlShader::getSelectionMask() const {
    return this->selMask;
}

//------------------------------------------------------------------------------
int32
mtlShader::getSelectionIndex() const {
    return this->selIndex;
}

//------------------------------------------------------------------------------
bool
mtlShader::select(uint32 mask) {
    if (this->selMask != mask) {
        for (int32 i = 0; i < this->numPrograms; i++) {
            if (this->programEntries[i].mask == mask) {
                this->selMask = mask;
                this->selIndex = i;
                return true;
            }
        }
    }
    return false;
}

//------------------------------------------------------------------------------
id<MTLFunction>
mtlShader::getSelectedVertexShader() const {
    return this->programEntries[this->selIndex].mtlVertexShader;
}

//------------------------------------------------------------------------------
id<MTLFunction>
mtlShader::getSelectedFragmentShader() const {
    return this->programEntries[this->selIndex].mtlFragmentShader;
}

//------------------------------------------------------------------------------
int32
mtlShader::getNumPrograms() const {
    return this->numPrograms;
}

//------------------------------------------------------------------------------
id<MTLFunction>
mtlShader::getVertexShaderAt(int32 index) const {
    return this->programEntries[index].mtlVertexShader;
}

//------------------------------------------------------------------------------
id<MTLFunction>
mtlShader::getFragmentShaderAt(int32 index) const {
    return this->programEntries[index].mtlFragmentShader;
}

//------------------------------------------------------------------------------
void
mtlShader::addUniformBlock(ShaderType::Code bindShaderStage, int32 bindSlotIndex) {
    ubEntry& entry = this->uniformBlockEntries[this->numUniformBlockEntries++];
    entry.bindShaderStage = bindShaderStage;
    entry.bindSlotIndex = bindSlotIndex;
}

//------------------------------------------------------------------------------
int32
mtlShader::getNumUniformBlocks() const {
    return this->numUniformBlockEntries;
}

//------------------------------------------------------------------------------
ShaderType::Code
mtlShader::getUniformBlockShaderStage(int32 ubIndex) const {
    return this->uniformBlockEntries[ubIndex].bindShaderStage;
}

//------------------------------------------------------------------------------
int32
mtlShader::getUniformBlockBindSlotIndex(int32 ubIndex) const {
    return this->uniformBlockEntries[ubIndex].bindSlotIndex;
}

} // namespace _priv
} // namespace Oryol