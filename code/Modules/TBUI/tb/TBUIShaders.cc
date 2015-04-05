//-----------------------------------------------------------------------------
// #version:15# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "TBUIShaders.h"

namespace Oryol {
namespace Shaders {
const char* vs_100_src = 
"#define _POSITION gl_Position\n"
"uniform mat4 ortho;\n"
"attribute vec4 position;\n"
"attribute vec2 texcoord0;\n"
"attribute vec4 color0;\n"
"varying vec2 uv;\n"
"varying vec4 color;\n"
"void main() {\n"
"_POSITION = ortho * position;\n"
"uv = texcoord0;\n"
"color = color0;\n"
"}\n"
;
const char* fs_100_src = 
"precision mediump float;\n"
"#define _COLOR gl_FragColor\n"
"#define _TEXTURE2D texture2D\n"
"uniform sampler2D tex;\n"
"varying vec2 uv;\n"
"varying vec4 color;\n"
"void main() {\n"
"_COLOR = _TEXTURE2D(tex, uv);\n"
"}\n"
;
const char* vs_120_src = 
"#version 120\n"
"#define _POSITION gl_Position\n"
"uniform mat4 ortho;\n"
"attribute vec4 position;\n"
"attribute vec2 texcoord0;\n"
"attribute vec4 color0;\n"
"varying vec2 uv;\n"
"varying vec4 color;\n"
"void main() {\n"
"_POSITION = ortho * position;\n"
"uv = texcoord0;\n"
"color = color0;\n"
"}\n"
;
const char* fs_120_src = 
"#version 120\n"
"#define _COLOR gl_FragColor\n"
"#define _TEXTURE2D texture2D\n"
"uniform sampler2D tex;\n"
"varying vec2 uv;\n"
"varying vec4 color;\n"
"void main() {\n"
"_COLOR = _TEXTURE2D(tex, uv);\n"
"}\n"
;
const char* vs_150_src = 
"#version 150\n"
"#define _POSITION gl_Position\n"
"uniform mat4 ortho;\n"
"in vec4 position;\n"
"in vec2 texcoord0;\n"
"in vec4 color0;\n"
"out vec2 uv;\n"
"out vec4 color;\n"
"void main() {\n"
"_POSITION = ortho * position;\n"
"uv = texcoord0;\n"
"color = color0;\n"
"}\n"
;
const char* fs_150_src = 
"#version 150\n"
"#define _COLOR _FragColor\n"
"#define _TEXTURE2D texture\n"
"uniform sampler2D tex;\n"
"in vec2 uv;\n"
"in vec4 color;\n"
"out vec4 _FragColor;\n"
"void main() {\n"
"_COLOR = _TEXTURE2D(tex, uv);\n"
"}\n"
;
ProgramBundleSetup TBUIShader::CreateSetup() {
    ProgramBundleSetup setup("TBUIShader");
    setup.AddProgramFromSources(0, ShaderLang::GLSL100, vs_100_src, fs_100_src);
    setup.AddProgramFromSources(0, ShaderLang::GLSL120, vs_120_src, fs_120_src);
    setup.AddProgramFromSources(0, ShaderLang::GLSL150, vs_150_src, fs_150_src);
    setup.AddUniform("ortho", Ortho);
    setup.AddTextureUniform("tex", Texture);
    return setup;
}
}
}

