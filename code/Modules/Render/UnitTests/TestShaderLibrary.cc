//-----------------------------------------------------------------------------
// #version:1# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "TestShaderLibrary.h"

namespace Oryol {
namespace TestShaderLibrary{
const char* MyVertexShader_100_src = 
"#define _POSITION gl_Position\n"
"uniform mat4 mvp;\n"
"attribute vec4 position;\n"
"attribute vec4 normal;\n"
"attribute vec2 texcoord0;\n"
"varying vec4 nrm;\n"
"varying vec2 uv;\n"
"vec4 myTransform(vec4 pos)\n"
"{\n"
"return mvp * pos;\n"
"}\n"
"void main()\n"
"{\n"
"_POSITION = myTransform(position);\n"
"nrm = normal;\n"
"uv  = texcoord0;\n"
"}\n"
;
const char* MyFragmentShader_100_src = 
"precision mediump float;\n"
"#define _TEXTURE2D texture2D\n"
"#define _COLOR gl_FragColor\n"
"uniform sampler2D tex;\n"
"varying vec4 nrm;\n"
"varying vec2 uv;\n"
"vec4 myColor(vec4 nrm, vec4 texColor)\n"
"{\n"
"return ((nrm * 0.5) + 0.5) * 0.75 + texColor * texColor * texColor;\n"
"}\n"
"void main()\n"
"{\n"
"vec4 texColor = _TEXTURE2D(tex, uv * vec2(5.0, 3.0));\n"
"_COLOR = myColor(nrm, texColor);\n"
"}\n"
;
const char* MyVertexShader_120_src = 
"#version 120\n"
"#define _POSITION gl_Position\n"
"uniform mat4 mvp;\n"
"attribute vec4 position;\n"
"attribute vec4 normal;\n"
"attribute vec2 texcoord0;\n"
"varying vec4 nrm;\n"
"varying vec2 uv;\n"
"vec4 myTransform(vec4 pos)\n"
"{\n"
"return mvp * pos;\n"
"}\n"
"void main()\n"
"{\n"
"_POSITION = myTransform(position);\n"
"nrm = normal;\n"
"uv  = texcoord0;\n"
"}\n"
;
const char* MyFragmentShader_120_src = 
"#version 120\n"
"#define _TEXTURE2D texture2D\n"
"#define _COLOR gl_FragColor\n"
"uniform sampler2D tex;\n"
"varying vec4 nrm;\n"
"varying vec2 uv;\n"
"vec4 myColor(vec4 nrm, vec4 texColor)\n"
"{\n"
"return ((nrm * 0.5) + 0.5) * 0.75 + texColor * texColor * texColor;\n"
"}\n"
"void main()\n"
"{\n"
"vec4 texColor = _TEXTURE2D(tex, uv * vec2(5.0, 3.0));\n"
"_COLOR = myColor(nrm, texColor);\n"
"}\n"
;
const char* MyVertexShader_150_src = 
"#version 150\n"
"#define _POSITION gl_Position\n"
"uniform mat4 mvp;\n"
"in vec4 position;\n"
"in vec4 normal;\n"
"in vec2 texcoord0;\n"
"out vec4 nrm;\n"
"out vec2 uv;\n"
"vec4 myTransform(vec4 pos)\n"
"{\n"
"return mvp * pos;\n"
"}\n"
"void main()\n"
"{\n"
"_POSITION = myTransform(position);\n"
"nrm = normal;\n"
"uv  = texcoord0;\n"
"}\n"
;
const char* MyFragmentShader_150_src = 
"#version 150\n"
"#define _TEXTURE2D texture\n"
"#define _COLOR _FragColor\n"
"uniform sampler2D tex;\n"
"in vec4 nrm;\n"
"in vec2 uv;\n"
"out vec4 _FragColor;\n"
"vec4 myColor(vec4 nrm, vec4 texColor)\n"
"{\n"
"return ((nrm * 0.5) + 0.5) * 0.75 + texColor * texColor * texColor;\n"
"}\n"
"void main()\n"
"{\n"
"vec4 texColor = _TEXTURE2D(tex, uv * vec2(5.0, 3.0));\n"
"_COLOR = myColor(nrm, texColor);\n"
"}\n"
;
Render::ProgramBundleSetup MyShader::CreateSetup() {
    Render::ProgramBundleSetup setup("MyShader");
    setup.AddProgramFromSources(0, Render::ShaderLang::GLSL100, MyVertexShader_100_src, MyFragmentShader_100_src);
    setup.AddProgramFromSources(0, Render::ShaderLang::GLSL120, MyVertexShader_120_src, MyFragmentShader_120_src);
    setup.AddProgramFromSources(0, Render::ShaderLang::GLSL150, MyVertexShader_150_src, MyFragmentShader_150_src);
    setup.AddUniform("mvp", ModelViewProj);
    setup.AddTextureUniform("tex", Texture);
    return setup;
}
}
}

