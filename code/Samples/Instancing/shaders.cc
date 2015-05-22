//-----------------------------------------------------------------------------
// #version:19# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "shaders.h"

namespace Oryol {
namespace Shaders {
#if ORYOL_OPENGL
const char* vs_glsl100_src = 
"#define mul(v,m) (m * v)\n"
"#define _POSITION gl_Position\n"
"uniform mat4 mvp;\n"
"attribute vec4 position;\n"
"attribute vec4 color0;\n"
"attribute vec4 instance0;\n"
"varying vec4 color;\n"
"void main() {\n"
"_POSITION = mvp * (position + instance0);\n"
"color = color0;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* fs_glsl100_src = 
"precision mediump float;\n"
"#define mul(v,m) (m * v)\n"
"#define _COLOR gl_FragColor\n"
"varying vec4 color;\n"
"void main() {\n"
"_COLOR = color;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* vs_glsl120_src = 
"#version 120\n"
"#define mul(v,m) (m * v)\n"
"#define _POSITION gl_Position\n"
"uniform mat4 mvp;\n"
"attribute vec4 position;\n"
"attribute vec4 color0;\n"
"attribute vec4 instance0;\n"
"varying vec4 color;\n"
"void main() {\n"
"_POSITION = mvp * (position + instance0);\n"
"color = color0;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* fs_glsl120_src = 
"#version 120\n"
"#define mul(v,m) (m * v)\n"
"#define _COLOR gl_FragColor\n"
"varying vec4 color;\n"
"void main() {\n"
"_COLOR = color;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* vs_glsl150_src = 
"#version 150\n"
"#define mul(v,m) (m * v)\n"
"#define _POSITION gl_Position\n"
"uniform mat4 mvp;\n"
"in vec4 position;\n"
"in vec4 color0;\n"
"in vec4 instance0;\n"
"out vec4 color;\n"
"void main() {\n"
"_POSITION = mvp * (position + instance0);\n"
"color = color0;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* fs_glsl150_src = 
"#version 150\n"
"#define mul(v,m) (m * v)\n"
"#define _COLOR _FragColor\n"
"in vec4 color;\n"
"out vec4 _FragColor;\n"
"void main() {\n"
"_COLOR = color;\n"
"}\n"
;
#endif
#if ORYOL_D3D11
const char* vs_hlsl5_src = 
"#define _POSITION _oPosition\n"
"#define mat4 float4x4\n"
"#define mat2 float2x2\n"
"#define mat3 float3x3\n"
"#define vec4 float4\n"
"#define vec2 float2\n"
"#define vec3 float3\n"
"mat4 mvp;\n"
"void main(\n"
"in vec4 position : position,\n"
"in vec4 color0 : color0,\n"
"in vec4 instance0 : instance0,\n"
"out vec4 color : color,\n"
"out vec4 _oPosition : SV_POSITION) {\n"
"_POSITION = mvp * (position + instance0);\n"
"color = color0;\n"
"}\n"
;
#endif
#if ORYOL_D3D11
const char* fs_hlsl5_src = 
"#define _COLOR _oColor\n"
"#define mat4 float4x4\n"
"#define mat2 float2x2\n"
"#define mat3 float3x3\n"
"#define vec4 float4\n"
"#define vec2 float2\n"
"#define vec3 float3\n"
"void main(\n"
"in vec4 color : color,\n"
"out vec4 _oColor : SV_TARGET) {\n"
"_COLOR = color;\n"
"}\n"
;
#endif
ProgramBundleSetup Main::CreateSetup() {
    ProgramBundleSetup setup("Main");
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL100, vs_glsl100_src, fs_glsl100_src);
    #endif
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL120, vs_glsl120_src, fs_glsl120_src);
    #endif
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL150, vs_glsl150_src, fs_glsl150_src);
    #endif
    #if ORYOL_D3D11
    setup.AddProgramFromSources(0, ShaderLang::HLSL5, vs_hlsl5_src, fs_hlsl5_src);
    #endif
    setup.AddUniform("mvp", ModelViewProjection);
    return setup;
}
}
}

