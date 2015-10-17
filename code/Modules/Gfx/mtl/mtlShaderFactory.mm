//------------------------------------------------------------------------------
//  mtlShaderFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlShaderFactory.h"
#include "Core/Assertion.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Resource/shader.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mtlShaderFactory::mtlShaderFactory() :
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
mtlShaderFactory::~mtlShaderFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
mtlShaderFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptrs;
}

//------------------------------------------------------------------------------
void
mtlShaderFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->pointers = gfxPointers();
    this->isValid = false;
}

//------------------------------------------------------------------------------
bool
mtlShaderFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlShaderFactory::SetupResource(shader& shd) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(nil == shd.getLibrary());

    const ShaderLang::Code slang = ShaderLang::Metal;
    const ShaderSetup& setup = shd.Setup;
    const void* libraryByteCode = nullptr;
    uint32 libraryByteCodeSize = 0;
    setup.LibraryByteCode(slang, libraryByteCode, libraryByteCodeSize);
    o_assert_dbg(nullptr != libraryByteCode);
    o_assert_dbg(libraryByteCodeSize > 0);

    // first create the shader library (one library per bundle)
    NSError* err = 0;
    dispatch_data_t libData = dispatch_data_create(libraryByteCode, libraryByteCodeSize, NULL, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
    id<MTLLibrary> mtlLibrary = [this->pointers.renderer->mtlDevice newLibraryWithData:libData error:&err];
    o_assert(nil == err);
    shd.setLibrary(mtlLibrary);

    // create vertex and fragment shader function objects
    const int32 numProgs = setup.NumPrograms();
    for (int32 progIndex = 0; progIndex < numProgs; progIndex++) {
        const String& vsName = setup.VertexShaderFunc(progIndex, slang);
        id<MTLFunction> vsFunc = [mtlLibrary newFunctionWithName:[NSString stringWithUTF8String:vsName.AsCStr()]];
        const String& fsName = setup.FragmentShaderFunc(progIndex, slang);
        id<MTLFunction> fsFunc = [mtlLibrary newFunctionWithName:[NSString stringWithUTF8String:fsName.AsCStr()]];
        shd.addShaders(setup.Mask(progIndex), vsFunc, fsFunc);
    }

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
mtlShaderFactory::DestroyResource(shader& shd) {
    o_assert_dbg(this->isValid);

    for (auto& entry : shd.programEntries) {
        if (nil != entry.mtlVertexShader) {
            ORYOL_OBJC_RELEASE(entry.vertexShader);
            entry.mtlVertexShader = nil;
        }
        if (nil != entry.mtlFragmentShader) {
            ORYOL_OBJC_RELEASE(entry.fragmentShader);
            entry.mtlFragmentShader = nil;
        }
    }
    if (nil != shd.mtlLibrary) {
        ORYOL_OBJC_RELEASE(shd.mtlLibrary);
        shd.mtlLibrary = nil;
    }
    shd.Clear();
}

} // namespace _priv
} // namespace Oryol
