//-----------------------------------------------------------------------------
// #version:1# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "shaders.h"

namespace Oryol {
namespace Shaders{
const char* renderTargetVS_100_src = 
"#define _POSITION gl_Position\n"
"uniform mat4 mvp;\n"
"attribute vec4 position;\n"
"attribute vec4 normal;\n"
"varying vec4 nrm;\n"
"void main() {\n"
"_POSITION = mvp * position;\n"
"nrm = normal;\n"
"}\n"
;
const char* mainVS_100_src = 
"#define _POSITION gl_Position\n"
"uniform mat4 mvp;\n"
"attribute vec4 position;\n"
"attribute vec4 normal;\n"
"attribute vec2 texcoord0;\n"
"varying vec4 nrm;\n"
"varying vec2 uv;\n"
"void main() {\n"
"_POSITION = mvp * position;\n"
"uv = texcoord0;\n"
"nrm = normalize(mvp * normal);\n"
"}\n"
;
const char* renderTargetFS_100_src = 
"precision mediump float;\n"
"#define _COLOR gl_FragColor\n"
"varying vec4 nrm;\n"
"void main() {\n"
"_COLOR = nrm * 0.5 + 0.5;\n"
"}\n"
;
const char* mainFS_100_src = 
"precision mediump float;\n"
"#define _TEXTURE2D texture2D\n"
"#define _COLOR gl_FragColor\n"
"uniform sampler2D tex;\n"
"varying vec2 uv;\n"
"varying vec4 nrm;\n"
"void main() {\n"
"vec4 c = _TEXTURE2D(tex, uv * vec2(20.0, 10.0));\n"
"float l = clamp(dot(nrm.xyz, normalize(vec3(1.0, 1.0, -1.0))), 0.0, 1.0) * 2.0;\n"
"_COLOR = c * (l + 0.25);\n"
"}\n"
;
const char* renderTargetVS_120_src = 
"#version 120\n"
"#define _POSITION gl_Position\n"
"uniform mat4 mvp;\n"
"attribute vec4 position;\n"
"attribute vec4 normal;\n"
"varying vec4 nrm;\n"
"void main() {\n"
"_POSITION = mvp * position;\n"
"nrm = normal;\n"
"}\n"
;
const char* mainVS_120_src = 
"#version 120\n"
"#define _POSITION gl_Position\n"
"uniform mat4 mvp;\n"
"attribute vec4 position;\n"
"attribute vec4 normal;\n"
"attribute vec2 texcoord0;\n"
"varying vec4 nrm;\n"
"varying vec2 uv;\n"
"void main() {\n"
"_POSITION = mvp * position;\n"
"uv = texcoord0;\n"
"nrm = normalize(mvp * normal);\n"
"}\n"
;
const char* renderTargetFS_120_src = 
"#version 120\n"
"#define _COLOR gl_FragColor\n"
"varying vec4 nrm;\n"
"void main() {\n"
"_COLOR = nrm * 0.5 + 0.5;\n"
"}\n"
;
const char* mainFS_120_src = 
"#version 120\n"
"#define _TEXTURE2D texture2D\n"
"#define _COLOR gl_FragColor\n"
"uniform sampler2D tex;\n"
"varying vec2 uv;\n"
"varying vec4 nrm;\n"
"void main() {\n"
"vec4 c = _TEXTURE2D(tex, uv * vec2(20.0, 10.0));\n"
"float l = clamp(dot(nrm.xyz, normalize(vec3(1.0, 1.0, -1.0))), 0.0, 1.0) * 2.0;\n"
"_COLOR = c * (l + 0.25);\n"
"}\n"
;
const char* renderTargetVS_150_src = 
"#version 150\n"
"#define _POSITION gl_Position\n"
"uniform mat4 mvp;\n"
"in vec4 position;\n"
"in vec4 normal;\n"
"out vec4 nrm;\n"
"void main() {\n"
"_POSITION = mvp * position;\n"
"nrm = normal;\n"
"}\n"
;
const char* mainVS_150_src = 
"#version 150\n"
"#define _POSITION gl_Position\n"
"uniform mat4 mvp;\n"
"in vec4 position;\n"
"in vec4 normal;\n"
"in vec2 texcoord0;\n"
"out vec4 nrm;\n"
"out vec2 uv;\n"
"void main() {\n"
"_POSITION = mvp * position;\n"
"uv = texcoord0;\n"
"nrm = normalize(mvp * normal);\n"
"}\n"
;
const char* renderTargetFS_150_src = 
"#version 150\n"
"#define _COLOR _FragColor\n"
"in vec4 nrm;\n"
"out vec4 _FragColor;\n"
"void main() {\n"
"_COLOR = nrm * 0.5 + 0.5;\n"
"}\n"
;
const char* mainFS_150_src = 
"#version 150\n"
"#define _TEXTURE2D texture\n"
"#define _COLOR _FragColor\n"
"uniform sampler2D tex;\n"
"in vec2 uv;\n"
"in vec4 nrm;\n"
"out vec4 _FragColor;\n"
"void main() {\n"
"vec4 c = _TEXTURE2D(tex, uv * vec2(20.0, 10.0));\n"
"float l = clamp(dot(nrm.xyz, normalize(vec3(1.0, 1.0, -1.0))), 0.0, 1.0) * 2.0;\n"
"_COLOR = c * (l + 0.25);\n"
"}\n"
;
Render::ProgramBundleSetup Main::CreateSetup() {
    Render::ProgramBundleSetup setup("Main");
    setup.AddProgramFromSources(0, Render::ShaderLang::GLSL100, mainVS_100_src, mainFS_100_src);
    setup.AddProgramFromSources(0, Render::ShaderLang::GLSL120, mainVS_120_src, mainFS_120_src);
    setup.AddProgramFromSources(0, Render::ShaderLang::GLSL150, mainVS_150_src, mainFS_150_src);
    setup.AddUniform("mvp", ModelViewProjection);
    setup.AddTextureUniform("tex", Texture);
    return setup;
}
Render::ProgramBundleSetup RenderTarget::CreateSetup() {
    Render::ProgramBundleSetup setup("RenderTarget");
    setup.AddProgramFromSources(0, Render::ShaderLang::GLSL100, renderTargetVS_100_src, renderTargetFS_100_src);
    setup.AddProgramFromSources(0, Render::ShaderLang::GLSL120, renderTargetVS_120_src, renderTargetFS_120_src);
    setup.AddProgramFromSources(0, Render::ShaderLang::GLSL150, renderTargetVS_150_src, renderTargetFS_150_src);
    setup.AddUniform("mvp", ModelViewProjection);
    return setup;
}
}
}

