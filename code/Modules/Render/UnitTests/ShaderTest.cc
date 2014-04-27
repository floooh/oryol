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
"VS_INPUT(vec4, position);\n"
"void main() {\n"
"  gl_Position = mvp * position;\n"
"}\n";

static const char* fsSource =
"void main() {\n"
"  FragmentColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
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
    RenderFacade* fac = RenderFacade::CreateSingle(RenderSetup::Windowed(400, 300, "ShaderCompileTest"));
    
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

    RenderFacade::DestroySingle();
}

//------------------------------------------------------------------------------
TEST(ProgramTest) {

    RenderFacade* facade = RenderFacade::CreateSingle(RenderSetup::Windowed(400, 400, "ProgramTest"));
    
    // setup a vertex- and fragment-shader
    Id vs = facade->CreateResource(ShaderSetup::FromSource("vstest", ShaderType::VertexShader, vsSource));
    Id fs = facade->CreateResource(ShaderSetup::FromSource("fstest", ShaderType::FragmentShader, fsSource));

    // setup program bundle from the vertex- and fragment shader
    ProgramBundleSetup progSetup("prog");
    progSetup.AddProgram(0, vs, fs);
    progSetup.AddUniform("mvp", 0); // note: ModelViewProh is normally a standard uniform
    CHECK(progSetup.GetNumPrograms() == 1);
    CHECK(progSetup.GetVertexShader(0) == vs);
    CHECK(progSetup.GetFragmentShader(0) == fs);
    CHECK(progSetup.GetNumUniforms() == 1);
    CHECK(progSetup.GetUniformName(0) == "mvp");
    CHECK(progSetup.GetUniformSlot(0) == 0);
    
    Id prog = facade->CreateResource(progSetup);
    CHECK(prog.IsValid());
    CHECK(prog.Type() == ResourceType::ProgramBundle);
    
    // apply the program
    facade->ApplyProgram(prog);
    
    // and discard the program
    facade->DiscardResource(prog);
    facade->DiscardResource(vs);
    facade->DiscardResource(fs);
    
    RenderFacade::DestroySingle();
}

} // namespace Render
} // namespace Oryol