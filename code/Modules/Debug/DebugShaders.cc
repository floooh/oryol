//-----------------------------------------------------------------------------
// #version:1# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "DebugShaders.h"

namespace Oryol {
namespace DebugShaders{
const char* vsText_100_src = 
"#define _POSITION gl_Position\n"
"uniform vec2 glyphSize;\n"
"attribute vec2 position;\n"
"attribute vec2 texcoord0;\n"
"varying vec2 uv;\n"
"void main() {\n"
"vec2 screenPos = position * glyphSize;\n"
"screenPos.x -= 1.0;\n"
"screenPos.y = 1.0 - screenPos.y;\n"
"_POSITION = vec4(screenPos, 0.0, 1.0);\n"
"uv = texcoord0 * vec2(8.0 / 1024.0, 1.0);\n"
"}\n"
;
const char* fsText_100_src = 
"precision mediump float;\n"
"#define _TEXTURE2D texture2D\n"
"#define _COLOR gl_FragColor\n"
"uniform sampler2D tex;\n"
"uniform vec4 textColor;\n"
"varying vec2 uv;\n"
"void main() {\n"
"_COLOR = _TEXTURE2D(tex, uv).xxxx * textColor;\n"
"}\n"
;
const char* vsText_120_src = 
"#version 120\n"
"#define _POSITION gl_Position\n"
"uniform vec2 glyphSize;\n"
"attribute vec2 position;\n"
"attribute vec2 texcoord0;\n"
"varying vec2 uv;\n"
"void main() {\n"
"vec2 screenPos = position * glyphSize;\n"
"screenPos.x -= 1.0;\n"
"screenPos.y = 1.0 - screenPos.y;\n"
"_POSITION = vec4(screenPos, 0.0, 1.0);\n"
"uv = texcoord0 * vec2(8.0 / 1024.0, 1.0);\n"
"}\n"
;
const char* fsText_120_src = 
"#version 120\n"
"#define _TEXTURE2D texture2D\n"
"#define _COLOR gl_FragColor\n"
"uniform sampler2D tex;\n"
"uniform vec4 textColor;\n"
"varying vec2 uv;\n"
"void main() {\n"
"_COLOR = _TEXTURE2D(tex, uv).xxxx * textColor;\n"
"}\n"
;
const char* vsText_150_src = 
"#version 150\n"
"#define _POSITION gl_Position\n"
"uniform vec2 glyphSize;\n"
"in vec2 position;\n"
"in vec2 texcoord0;\n"
"out vec2 uv;\n"
"void main() {\n"
"vec2 screenPos = position * glyphSize;\n"
"screenPos.x -= 1.0;\n"
"screenPos.y = 1.0 - screenPos.y;\n"
"_POSITION = vec4(screenPos, 0.0, 1.0);\n"
"uv = texcoord0 * vec2(8.0 / 1024.0, 1.0);\n"
"}\n"
;
const char* fsText_150_src = 
"#version 150\n"
"#define _TEXTURE2D texture\n"
"#define _COLOR _FragColor\n"
"uniform sampler2D tex;\n"
"uniform vec4 textColor;\n"
"in vec2 uv;\n"
"out vec4 _FragColor;\n"
"void main() {\n"
"_COLOR = _TEXTURE2D(tex, uv).xxxx * textColor;\n"
"}\n"
;
Render::ProgramBundleSetup TextShader::CreateSetup() {
    Render::ProgramBundleSetup setup("TextShader");
    setup.AddProgramFromSources(0, Render::ShaderLang::GLSL100, vsText_100_src, fsText_100_src);
    setup.AddProgramFromSources(0, Render::ShaderLang::GLSL120, vsText_120_src, fsText_120_src);
    setup.AddProgramFromSources(0, Render::ShaderLang::GLSL150, vsText_150_src, fsText_150_src);
    setup.AddUniform("glyphSize", GlyphSize);
    setup.AddTextureUniform("tex", Texture);
    setup.AddUniform("textColor", TextColor);
    return setup;
}
}
}

