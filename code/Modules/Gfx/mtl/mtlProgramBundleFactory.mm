//------------------------------------------------------------------------------
//  mtlProgramBundleFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlProgramBundleFactory.h"
#include "Core/Assertion.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Resource/programBundle.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mtlProgramBundleFactory::mtlProgramBundleFactory() :
renderer(0),
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
mtlProgramBundleFactory::~mtlProgramBundleFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
mtlProgramBundleFactory::Setup(class renderer* rendr) {
    o_assert_dbg(!this->isValid);
    o_assert_dbg(nullptr != rendr);
    this->isValid = true;
    this->renderer = rendr;
}

//------------------------------------------------------------------------------
void
mtlProgramBundleFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    this->renderer = nullptr;
}

//------------------------------------------------------------------------------
bool
mtlProgramBundleFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlProgramBundleFactory::SetupResource(programBundle& progBundle) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(this->renderer && this->renderer->mtlDevice);
    o_assert_dbg(nil == progBundle.getLibrary());

    this->renderer->invalidateProgramState();
    const ShaderLang::Code slang = ShaderLang::Metal;
    const ProgramBundleSetup& setup = progBundle.Setup;
    const void* libraryByteCode = nullptr;
    uint32 libraryByteCodeSize = 0;
    setup.LibraryByteCode(slang, libraryByteCode, libraryByteCodeSize);
    o_assert_dbg(nullptr != libraryByteCode);
    o_assert_dbg(libraryByteCodeSize > 0);

    // first create the shader library (one library per bundle)
    NSError* err = 0;
    dispatch_data_t libData = dispatch_data_create(libraryByteCode, libraryByteCodeSize, NULL, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
    id<MTLLibrary> mtlLibrary = [this->renderer->mtlDevice newLibraryWithData:libData error:&err];
    progBundle.setLibrary(mtlLibrary);
    o_assert(nil == err);

    // create vertex and fragment shader function objects
    const int32 numProgs = setup.NumPrograms();
    for (int32 progIndex = 0; progIndex < numProgs; progIndex++) {

        // create vertex shader
        const String& vsName = setup.VertexShaderFunc(progIndex, slang);
        id<MTLFunction> vsFunc = [mtlLibrary newFunctionWithName:[NSString stringWithUTF8String:vsName.AsCStr()]];
        const String& fsName = setup.FragmentShaderFunc(progIndex, slang);
        id<MTLFunction> fsFunc = [mtlLibrary newFunctionWithName:[NSString stringWithUTF8String:fsName.AsCStr()]];
        progBundle.addShaders(setup.Mask(progIndex), vsFunc, fsFunc);
    }

    // setup uniform block binding data
    for (int i = 0; i < setup.NumUniformBlocks(); i++) {
        const ShaderType::Code bindShaderStage = setup.UniformBlockShaderStage(i);
        const int32 bindSlotIndex = setup.UniformBlockSlot(i);
        progBundle.addUniformBlock(bindShaderStage, bindSlotIndex);
    }

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
mtlProgramBundleFactory::DestroyResource(programBundle& progBundle) {
    o_assert_dbg(this->isValid);

    this->renderer->invalidateProgramState();
    for (auto& entry : progBundle.programEntries) {
        if (nil != entry.mtlVertexShader) {
            ORYOL_OBJC_RELEASE(entry.vertexShader);
            entry.mtlVertexShader = nil;
        }
        if (nil != entry.mtlFragmentShader) {
            ORYOL_OBJC_RELEASE(entry.fragmentShader);
            entry.mtlFragmentShader = nil;
        }
    }
    if (nil != progBundle.mtlLibrary) {
        ORYOL_OBJC_RELEASE(progBundle.mtlLibrary);
        progBundle.mtlLibrary = nil;
    }
    progBundle.Clear();
}

} // namespace _priv
} // namespace Oryol
