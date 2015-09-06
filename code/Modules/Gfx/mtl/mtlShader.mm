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
    this->numPrograms = 0;
    this->programEntries.Fill(programEntry());
    this->mtlLibrary = nil;
    this->numUniformBlockEntries = 0;
    this->uniformBlockEntries.Fill(ubEntry());
    this->numVSTextures = 0;
    this->vsTextures.Fill(texEntry());
    this->numFSTextures = 0;
    this->fsTextures.Fill(texEntry());
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
int32
mtlShader::getProgIndexByMask(uint32 mask) const {
    for (int32 i = 0; i < this->numPrograms; i++) {
        if (this->programEntries[i].mask == mask) {
            return i;
        }
    }
    return InvalidIndex;
}

//------------------------------------------------------------------------------
int32
mtlShader::getNumPrograms() const {
    return this->numPrograms;
}

//------------------------------------------------------------------------------
id<MTLFunction>
mtlShader::getVertexShader(int32 progIndex) const {
    return this->programEntries[progIndex].mtlVertexShader;
}

//------------------------------------------------------------------------------
id<MTLFunction>
mtlShader::getFragmentShader(int32 progIndex) const {
    return this->programEntries[progIndex].mtlFragmentShader;
}

//------------------------------------------------------------------------------
void
mtlShader::addUniformBlock(ShaderStage::Code bindShaderStage, int32 bindSlotIndex) {
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
ShaderStage::Code
mtlShader::getUniformBlockShaderStage(int32 ubIndex) const {
    return this->uniformBlockEntries[ubIndex].bindShaderStage;
}

//------------------------------------------------------------------------------
int32
mtlShader::getUniformBlockBindSlotIndex(int32 ubIndex) const {
    return this->uniformBlockEntries[ubIndex].bindSlotIndex;
}

//------------------------------------------------------------------------------
void
mtlShader::addTexture(ShaderStage::Code stage, TextureType::Code type, int32 bindSlotIndex) {
    texEntry& entry = stage == ShaderStage::VS ? this->vsTextures[this->numVSTextures++] : this->fsTextures[this->numFSTextures++];
    entry.type = type;
    entry.bindSlotIndex = bindSlotIndex;
}

//------------------------------------------------------------------------------
int32
mtlShader::getNumTextures(ShaderStage::Code stage) const {
    return stage == ShaderStage::VS ? this->numVSTextures : this->numFSTextures;
}

//------------------------------------------------------------------------------
TextureType::Code
mtlShader::getTextureType(ShaderStage::Code stage, int32 texIndex) const {
    return stage == ShaderStage::VS ? this->vsTextures[texIndex].type : this->fsTextures[texIndex].type;
}

//------------------------------------------------------------------------------
int32
mtlShader::getTextureBindSlotIndex(ShaderStage::Code stage, int32 texIndex) const {
    return stage == ShaderStage::VS ? this->vsTextures[texIndex].bindSlotIndex : this->fsTextures[texIndex].bindSlotIndex;
}

} // namespace _priv
} // namespace Oryol
