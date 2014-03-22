//------------------------------------------------------------------------------
//  ShaderTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Render/RenderFacade.h"

namespace Oryol {
namespace Render {
    
using namespace Core;
using namespace Resource;

static const char* vsSource =
"uniform mat4 mvp;\n"
"#if ORYOL_OPENGL\n"
"in vec4 position;\n"
"#else\n"
"attribute vec4 position;\n"
"#endif\n"
"void main() {\n"
"  gl_Position = mvp * position;\n"
"}\n";

static const char* fsSource =
"#if ORYOL_OPENGL\n"
"out vec4 colorOut;\n"
"#else\n"
"#define colorOut gl_FragColor\n"
"#endif\n"
"void main() {\n"
"  colorOut = vec4(1.0, 0.0, 0.0, 1.0);\n"
"}\n";

//------------------------------------------------------------------------------
TEST(ShaderFactoryTest) {
    // setup a GL context
    auto renderSetup = RenderSetup::Windowed(400, 300, "Oryol Test");
    displayMgr displayManager;
    displayManager.SetupDisplay(renderSetup);
    
    // setup a meshFactory object
    shaderFactory factory;
    factory.Setup();

    shader shd;
    CHECK(!shd.GetId().IsValid());
    CHECK(shd.GetState() == Resource::State::Initial);
    CHECK(shd.GetShaderType() == ShaderType::InvalidShaderType);
    CHECK(shd.glGetShader() == 0);
    
    shd.setSetup(ShaderSetup::FromSource("vstest", ShaderType::VertexShader, vsSource));
    shd.setState(Resource::State::Setup);
    factory.SetupResource(shd);
    
    CHECK(!shd.GetId().IsValid());
    CHECK(shd.GetState() == Resource::State::Valid);
    CHECK(shd.GetShaderType() == ShaderType::VertexShader);
    CHECK(shd.glGetShader() != 0);
    
    factory.DestroyResource(shd);
    CHECK(!shd.GetId().IsValid());
    CHECK(shd.GetState() == Resource::State::Setup);
    CHECK(shd.GetShaderType() == ShaderType::InvalidShaderType);
    CHECK(shd.glGetShader() == 0);
    
    factory.Discard();
    displayManager.DiscardDisplay();
}

//------------------------------------------------------------------------------
TEST(ShaderCreationTest) {

    // setup render facade object
    RenderFacade* fac = RenderFacade::CreateSingleton();
    auto dispSetup = RenderSetup::Windowed(400, 300, "ShaderCompileTest");
    fac->Setup(dispSetup);
    CHECK(fac->IsValid());
    
    // setup vertex shader
    const Locator vsLoc("vstest");
    ShaderSetup vsSetup = ShaderSetup::FromSource(vsLoc, ShaderType::VertexShader, vsSource);
    CHECK(vsSetup.GetLocator() == vsLoc);
    CHECK(vsSetup.GetType() == ShaderType::VertexShader);
    CHECK(vsSetup.GetSource().IsValid());
    CHECK(vsSetup.GetDefines().Empty());
    
    const Locator fsLoc("fstest");
    ShaderSetup fsSetup = ShaderSetup::FromSource(fsLoc, ShaderType::FragmentShader, fsSource);
    CHECK(fsSetup.GetLocator() == fsLoc);
    CHECK(fsSetup.GetType() == ShaderType::FragmentShader);
    CHECK(fsSetup.GetSource().IsValid());
    CHECK(fsSetup.GetDefines().Empty());
    
    Id vs = fac->CreateResource(vsSetup);
    Id fs = fac->CreateResource(fsSetup);
    CHECK(vs.IsValid());
    CHECK(vs.Type() == ResourceType::Shader);
    CHECK(fs.IsValid());
    CHECK(fs.Type() == ResourceType::Shader);
    CHECK(fac->LookupResource(vsLoc) == vs);
    CHECK(fac->LookupResource(fsLoc) == fs);
    fac->DiscardResource(vs);
    fac->DiscardResource(vs);
    fac->DiscardResource(fs);
    fac->DiscardResource(fs);
    CHECK(!fac->LookupResource(vsLoc).IsValid());
    CHECK(!fac->LookupResource(fsLoc).IsValid());

    RenderFacade::DestroySingleton();
}

} // namespace Render
} // namespace Oryol