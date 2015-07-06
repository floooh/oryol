//------------------------------------------------------------------------------
//  mtlProgramBundle.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlProgramBundle.h"
#include "mtl_impl.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mtlProgramBundle::mtlProgramBundle() {
    this->Clear();
}

//------------------------------------------------------------------------------
mtlProgramBundle::~mtlProgramBundle() {
#if ORYOL_DEBUG
    for (int32 i = 0; i < this->numPrograms; i++) {
        o_assert_dbg(nil == this->programEntries[i].vertexShader);
        o_assert_dbg(nil == this->programEntries[i].fragmentShader);
    }
#endif
}

//------------------------------------------------------------------------------
void
mtlProgramBundle::Clear() {
    this->selMask = 0xFFFFFFFF;
    this->selIndex = 0;
    this->numPrograms = 0;
    this->programEntries.Fill(programEntry());
    this->library = nil;
    programBundleBase::Clear();
}

//------------------------------------------------------------------------------
void
mtlProgramBundle::setLibrary(id<MTLLibrary> lib) {
    o_assert_dbg(nil != lib);
    this->library = lib;
}

//------------------------------------------------------------------------------
id<MTLLibrary>
mtlProgramBundle::getLibrary() const {
    return this->library;
}

//------------------------------------------------------------------------------
int32
mtlProgramBundle::addShaders(uint32 mask, id<MTLFunction> vsFunc, id<MTLFunction> fsFunc) {
    o_assert_dbg(this->numPrograms < ProgramBundleSetup::MaxNumPrograms);
    o_assert_dbg(nil != vsFunc);
    o_assert_dbg(nil != fsFunc);

    // make sure the mask is unique
    for (int32 i = 0; i < this->numPrograms; i++) {
        o_assert_dbg(this->programEntries[i].mask != mask);
    }

    auto& entry = this->programEntries[this->numPrograms];
    entry.mask = mask;
    entry.vertexShader = vsFunc;
    entry.fragmentShader = fsFunc;

    return this->numPrograms++;
}

//------------------------------------------------------------------------------
uint32
mtlProgramBundle::getSelectionMask() const {
    return this->selMask;
}

//------------------------------------------------------------------------------
int32
mtlProgramBundle::getSelectionIndex() const {
    return this->selIndex;
}

//------------------------------------------------------------------------------
bool
mtlProgramBundle::select(uint32 mask) {
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
mtlProgramBundle::getSelectedVertexShader() const {
    return this->programEntries[this->selIndex].vertexShader;
}

//------------------------------------------------------------------------------
id<MTLFunction>
mtlProgramBundle::getSelectedFragmentShader() const {
    return this->programEntries[this->selIndex].fragmentShader;
}

//------------------------------------------------------------------------------
int32
mtlProgramBundle::getNumPrograms() const {
    return this->numPrograms;
}

//------------------------------------------------------------------------------
id<MTLFunction>
mtlProgramBundle::getVertexShaderAt(int32 index) const {
    return this->programEntries[index].vertexShader;
}

//------------------------------------------------------------------------------
id<MTLFunction>
mtlProgramBundle::getFragmentShaderAt(int32 index) const {
    return this->programEntries[index].fragmentShader;
}

} // namespace _priv
} // namespace Oryol