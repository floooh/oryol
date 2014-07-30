//-----------------------------------------------------------------------------
// #version:1# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "shaders.h"

namespace Oryol {
namespace Shaders{
const char* drawVS_100_src = 
"#define _POSITION gl_Position\n"
"uniform mat4 mvp;\n"
"attribute vec4 position;\n"
"attribute vec4 color0;\n"
"attribute float instance0;\n"
"varying vec4 color;\n"
"void main() {\n"
"_POSITION = mvp * position;\n"
"color = color0;\n"
"}\n"
;
const char* emitVS_100_src = 
"#define _POSITION gl_Position\n"
"attribute vec4 position;\n"
"attribute vec2 texcoord0;\n"
"varying vec2 uv;\n"
"void main() {\n"
"_POSITION = position;\n"
"uv = texcoord0;\n"
"}\n"
;
const char* updateVS_100_src = 
"#define _POSITION gl_Position\n"
"attribute vec4 position;\n"
"attribute vec2 texcoord0;\n"
"varying vec2 uv;\n"
"void main() {\n"
"_POSITION = position;\n"
"uv = texcoord0;\n"
"}\n"
;
const char* drawFS_100_src = 
"precision mediump float;\n"
"#define _COLOR gl_FragColor\n"
"varying vec4 color;\n"
"void main() {\n"
"_COLOR = color;\n"
"}\n"
;
const char* emitFS_100_src = 
"precision mediump float;\n"
"#define _COLOR gl_FragColor\n"
"uniform float time;\n"
"varying vec2 uv;\n"
"void main() {\n"
"_COLOR = vec4(1.0, 0.0, 0.0, 1.0);\n"
"}\n"
;
const char* updateFS_100_src = 
"precision mediump float;\n"
"#define _COLOR gl_FragColor\n"
"varying vec2 uv;\n"
"void main() {\n"
"_COLOR = vec4(0.0, 1.0, 0.0, 1.0);\n"
"}\n"
;
const char* drawVS_120_src = 
"#version 120\n"
"#define _POSITION gl_Position\n"
"uniform mat4 mvp;\n"
"attribute vec4 position;\n"
"attribute vec4 color0;\n"
"attribute float instance0;\n"
"varying vec4 color;\n"
"void main() {\n"
"_POSITION = mvp * position;\n"
"color = color0;\n"
"}\n"
;
const char* emitVS_120_src = 
"#version 120\n"
"#define _POSITION gl_Position\n"
"attribute vec4 position;\n"
"attribute vec2 texcoord0;\n"
"varying vec2 uv;\n"
"void main() {\n"
"_POSITION = position;\n"
"uv = texcoord0;\n"
"}\n"
;
const char* updateVS_120_src = 
"#version 120\n"
"#define _POSITION gl_Position\n"
"attribute vec4 position;\n"
"attribute vec2 texcoord0;\n"
"varying vec2 uv;\n"
"void main() {\n"
"_POSITION = position;\n"
"uv = texcoord0;\n"
"}\n"
;
const char* drawFS_120_src = 
"#version 120\n"
"#define _COLOR gl_FragColor\n"
"varying vec4 color;\n"
"void main() {\n"
"_COLOR = color;\n"
"}\n"
;
const char* emitFS_120_src = 
"#version 120\n"
"#define _COLOR gl_FragColor\n"
"uniform float time;\n"
"varying vec2 uv;\n"
"void main() {\n"
"_COLOR = vec4(1.0, 0.0, 0.0, 1.0);\n"
"}\n"
;
const char* updateFS_120_src = 
"#version 120\n"
"#define _COLOR gl_FragColor\n"
"varying vec2 uv;\n"
"void main() {\n"
"_COLOR = vec4(0.0, 1.0, 0.0, 1.0);\n"
"}\n"
;
const char* drawVS_150_src = 
"#version 150\n"
"#define _POSITION gl_Position\n"
"uniform mat4 mvp;\n"
"in vec4 position;\n"
"in vec4 color0;\n"
"in float instance0;\n"
"out vec4 color;\n"
"void main() {\n"
"_POSITION = mvp * position;\n"
"color = color0;\n"
"}\n"
;
const char* emitVS_150_src = 
"#version 150\n"
"#define _POSITION gl_Position\n"
"in vec4 position;\n"
"in vec2 texcoord0;\n"
"out vec2 uv;\n"
"void main() {\n"
"_POSITION = position;\n"
"uv = texcoord0;\n"
"}\n"
;
const char* updateVS_150_src = 
"#version 150\n"
"#define _POSITION gl_Position\n"
"in vec4 position;\n"
"in vec2 texcoord0;\n"
"out vec2 uv;\n"
"void main() {\n"
"_POSITION = position;\n"
"uv = texcoord0;\n"
"}\n"
;
const char* drawFS_150_src = 
"#version 150\n"
"#define _COLOR _FragColor\n"
"in vec4 color;\n"
"out vec4 _FragColor;\n"
"void main() {\n"
"_COLOR = color;\n"
"}\n"
;
const char* emitFS_150_src = 
"#version 150\n"
"#define _COLOR _FragColor\n"
"uniform float time;\n"
"in vec2 uv;\n"
"out vec4 _FragColor;\n"
"void main() {\n"
"_COLOR = vec4(1.0, 0.0, 0.0, 1.0);\n"
"}\n"
;
const char* updateFS_150_src = 
"#version 150\n"
"#define _COLOR _FragColor\n"
"in vec2 uv;\n"
"out vec4 _FragColor;\n"
"void main() {\n"
"_COLOR = vec4(0.0, 1.0, 0.0, 1.0);\n"
"}\n"
;
Render::ProgramBundleSetup DrawParticles::CreateSetup() {
    Render::ProgramBundleSetup setup("DrawParticles");
    setup.AddProgramFromSources(0, Render::ShaderLang::GLSL100, drawVS_100_src, drawFS_100_src);
    setup.AddProgramFromSources(0, Render::ShaderLang::GLSL120, drawVS_120_src, drawFS_120_src);
    setup.AddProgramFromSources(0, Render::ShaderLang::GLSL150, drawVS_150_src, drawFS_150_src);
    setup.AddUniform("mvp", ModelViewProjection);
    return setup;
}
Render::ProgramBundleSetup EmitParticles::CreateSetup() {
    Render::ProgramBundleSetup setup("EmitParticles");
    setup.AddProgramFromSources(0, Render::ShaderLang::GLSL100, emitVS_100_src, emitFS_100_src);
    setup.AddProgramFromSources(0, Render::ShaderLang::GLSL120, emitVS_120_src, emitFS_120_src);
    setup.AddProgramFromSources(0, Render::ShaderLang::GLSL150, emitVS_150_src, emitFS_150_src);
    setup.AddUniform("time", Time);
    return setup;
}
Render::ProgramBundleSetup UpdateParticles::CreateSetup() {
    Render::ProgramBundleSetup setup("UpdateParticles");
    setup.AddProgramFromSources(0, Render::ShaderLang::GLSL100, updateVS_100_src, updateFS_100_src);
    setup.AddProgramFromSources(0, Render::ShaderLang::GLSL120, updateVS_120_src, updateFS_120_src);
    setup.AddProgramFromSources(0, Render::ShaderLang::GLSL150, updateVS_150_src, updateFS_150_src);
    return setup;
}
}
}

