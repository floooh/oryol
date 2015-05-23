//-----------------------------------------------------------------------------
// #version:22# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "shaders.h"

namespace Oryol {
namespace Shaders {
#if ORYOL_OPENGL
const char* vs_glsl100_src = 
"#define tex2Dvs(s, t) texture2D(s, t)\n"
"#define mul(v,m) (m * v)\n"
"#define tex2D(s, t) texture2D(s, t)\n"
"#define static \n"
"#define _color gl_FragColor\n"
"#define texCUBE(s, t) textureCube(s, t)\n"
"#define _position gl_Position\n"
"uniform mat4 mvp;\n"
"uniform vec4 particleTranslate;\n"
"attribute vec4 position;\n"
"attribute vec4 color0;\n"
"varying vec4 color;\n"
"void main() {\n"
"_position = mul((position + particleTranslate), mvp);\n"
"color = color0;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* fs_glsl100_src = 
"precision mediump float;\n"
"#define tex2Dvs(s, t) texture2D(s, t)\n"
"#define mul(v,m) (m * v)\n"
"#define tex2D(s, t) texture2D(s, t)\n"
"#define static \n"
"#define _color gl_FragColor\n"
"#define texCUBE(s, t) textureCube(s, t)\n"
"#define _position gl_Position\n"
"varying vec4 color;\n"
"void main() {\n"
"_color = color;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* vs_glsl120_src = 
"#version 120\n"
"#define tex2Dvs(s, t) texture2D(s, t)\n"
"#define mul(v,m) (m * v)\n"
"#define tex2D(s, t) texture2D(s, t)\n"
"#define static \n"
"#define _color gl_FragColor\n"
"#define texCUBE(s, t) textureCube(s, t)\n"
"#define _position gl_Position\n"
"uniform mat4 mvp;\n"
"uniform vec4 particleTranslate;\n"
"attribute vec4 position;\n"
"attribute vec4 color0;\n"
"varying vec4 color;\n"
"void main() {\n"
"_position = mul((position + particleTranslate), mvp);\n"
"color = color0;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* fs_glsl120_src = 
"#version 120\n"
"#define tex2Dvs(s, t) texture2D(s, t)\n"
"#define mul(v,m) (m * v)\n"
"#define tex2D(s, t) texture2D(s, t)\n"
"#define static \n"
"#define _color gl_FragColor\n"
"#define texCUBE(s, t) textureCube(s, t)\n"
"#define _position gl_Position\n"
"varying vec4 color;\n"
"void main() {\n"
"_color = color;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* vs_glsl150_src = 
"#version 150\n"
"#define tex2Dvs(s, t) texture(s, t)\n"
"#define mul(v,m) (m * v)\n"
"#define tex2D(s, t) texture(s, t)\n"
"#define static \n"
"#define _color _FragColor\n"
"#define texCUBE(s, t) texture(s, t)\n"
"#define _position gl_Position\n"
"uniform mat4 mvp;\n"
"uniform vec4 particleTranslate;\n"
"in vec4 position;\n"
"in vec4 color0;\n"
"out vec4 color;\n"
"void main() {\n"
"_position = mul((position + particleTranslate), mvp);\n"
"color = color0;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* fs_glsl150_src = 
"#version 150\n"
"#define tex2Dvs(s, t) texture(s, t)\n"
"#define mul(v,m) (m * v)\n"
"#define tex2D(s, t) texture(s, t)\n"
"#define static \n"
"#define _color _FragColor\n"
"#define texCUBE(s, t) texture(s, t)\n"
"#define _position gl_Position\n"
"in vec4 color;\n"
"out vec4 _FragColor;\n"
"void main() {\n"
"_color = color;\n"
"}\n"
;
#endif
#if ORYOL_D3D11
const char* vs_hlsl5_src = 
"#define mod(x,y) (x-y*floor(x/y))\n"
"#define fract(x) frac(x)\n"
"#define vec4 float4\n"
"#define vec2 float2\n"
"#define vec3 float3\n"
"#define texCUBE(s, t) s.Sample(s ## _sampler, t)\n"
"#define _position _oPosition\n"
"#define mat4 float4x4\n"
"#define mat2 float2x2\n"
"#define mat3 float3x3\n"
"#define tex2D(s, t) s.Sample(s ## _sampler, t)\n"
"#define mix(a,b,c) lerp(a,b,c)\n"
"#define _color _oColor\n"
"#define tex2Dvs(s, t) s.Load(int3(t.x, t.y, 0))\n"
"mat4 mvp;\n"
"vec4 particleTranslate;\n"
"void main(\n"
"in vec4 position : position,\n"
"in vec4 color0 : color0,\n"
"out vec4 color : color,\n"
"out vec4 _oPosition : SV_POSITION) {\n"
"_position = mul((position + particleTranslate), mvp);\n"
"color = color0;\n"
"}\n"
;
#endif
#if ORYOL_D3D11
const char* fs_hlsl5_src = 
"#define mod(x,y) (x-y*floor(x/y))\n"
"#define fract(x) frac(x)\n"
"#define vec4 float4\n"
"#define vec2 float2\n"
"#define vec3 float3\n"
"#define texCUBE(s, t) s.Sample(s ## _sampler, t)\n"
"#define _position _oPosition\n"
"#define mat4 float4x4\n"
"#define mat2 float2x2\n"
"#define mat3 float3x3\n"
"#define tex2D(s, t) s.Sample(s ## _sampler, t)\n"
"#define mix(a,b,c) lerp(a,b,c)\n"
"#define _color _oColor\n"
"#define tex2Dvs(s, t) s.Load(int3(t.x, t.y, 0))\n"
"void main(\n"
"in vec4 color : color,\n"
"out vec4 _oColor : SV_TARGET) {\n"
"_color = color;\n"
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
    setup.AddUniform("particleTranslate", ParticleTranslate);
    return setup;
}
}
}

