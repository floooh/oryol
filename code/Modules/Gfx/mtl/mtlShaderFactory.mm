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
    o_assert_dbg(nil == shd.mtlLibrary);

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
    shd.mtlLibrary = [this->pointers.renderer->mtlDevice newLibraryWithData:libData error:&err];
    o_assert(nil == err);

    // create vertex and fragment shader function objects
    const String& vsName = setup.VertexShaderFunc(slang);
    shd.mtlVertexShader = [shd.mtlLibrary newFunctionWithName:[NSString stringWithUTF8String:vsName.AsCStr()]];
    const String& fsName = setup.FragmentShaderFunc(slang);
    shd.mtlFragmentShader = [shd.mtlLibrary newFunctionWithName:[NSString stringWithUTF8String:fsName.AsCStr()]];

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
mtlShaderFactory::DestroyResource(shader& shd) {
    o_assert_dbg(this->isValid);
    if (nil != shd.mtlVertexShader) {
        this->pointers.renderer->releaseDeferred(shd.mtlVertexShader);
    }
    if (nil != shd.mtlFragmentShader) {
        this->pointers.renderer->releaseDeferred(shd.mtlFragmentShader);
    }
    if (nil != shd.mtlLibrary) {
        this->pointers.renderer->releaseDeferred(shd.mtlLibrary);
    }
    shd.Clear();
}

} // namespace _priv
} // namespace Oryol
