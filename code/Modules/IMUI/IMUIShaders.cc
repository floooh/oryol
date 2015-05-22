//-----------------------------------------------------------------------------
// #version:17# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "IMUIShaders.h"

namespace Oryol {
namespace Shaders {
#if ORYOL_OPENGL
const char* vsIMUI_glsl100_src = 
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
#endif
#if ORYOL_OPENGL
const char* fsIMUI_glsl100_src = 
"precision mediump float;\n"
"#define _TEXTURE2D texture2D\n"
"#define _COLOR gl_FragColor\n"
"uniform sampler2D tex;\n"
"varying vec2 uv;\n"
"varying vec4 color;\n"
"void main() {\n"
"_COLOR = _TEXTURE2D(tex, uv).xxxx * color;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* vsIMUI_glsl120_src = 
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
#endif
#if ORYOL_OPENGL
const char* fsIMUI_glsl120_src = 
"#version 120\n"
"#define _TEXTURE2D texture2D\n"
"#define _COLOR gl_FragColor\n"
"uniform sampler2D tex;\n"
"varying vec2 uv;\n"
"varying vec4 color;\n"
"void main() {\n"
"_COLOR = _TEXTURE2D(tex, uv).xxxx * color;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* vsIMUI_glsl150_src = 
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
#endif
#if ORYOL_OPENGL
const char* fsIMUI_glsl150_src = 
"#version 150\n"
"#define _TEXTURE2D texture\n"
"#define _COLOR _FragColor\n"
"uniform sampler2D tex;\n"
"in vec2 uv;\n"
"in vec4 color;\n"
"out vec4 _FragColor;\n"
"void main() {\n"
"_COLOR = _TEXTURE2D(tex, uv).xxxx * color;\n"
"}\n"
;
#endif
ProgramBundleSetup IMUIShader::CreateSetup() {
    ProgramBundleSetup setup("IMUIShader");
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL100, vsIMUI_glsl100_src, fsIMUI_glsl100_src);
    #endif
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL120, vsIMUI_glsl120_src, fsIMUI_glsl120_src);
    #endif
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL150, vsIMUI_glsl150_src, fsIMUI_glsl150_src);
    #endif
    #if ORYOL_D3D11
    setup.AddProgramFromSources(0, ShaderLang::HLSL5, vsIMUI_hlsl5_src, fsIMUI_hlsl5_src);
    #endif
    setup.AddUniform("ortho", Ortho);
    setup.AddTextureUniform("tex", Texture);
    return setup;
}
}
}

