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

} // namespace _priv
} // namespace Oryol
