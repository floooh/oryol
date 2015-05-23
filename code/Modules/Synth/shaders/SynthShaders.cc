//-----------------------------------------------------------------------------
// #version:22# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "SynthShaders.h"

namespace Oryol {
namespace Shaders {
#if ORYOL_OPENGL
const char* vsSynth_glsl100_src = 
"#define tex2Dvs(s, t) texture2D(s, t)\n"
"#define mul(v,m) (m * v)\n"
"#define tex2D(s, t) texture2D(s, t)\n"
"#define static \n"
"#define _color gl_FragColor\n"
"#define texCUBE(s, t) textureCube(s, t)\n"
"#define _position gl_Position\n"
"attribute vec4 position;\n"
"void main() {\n"
"_position = position;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* fsSynth_glsl100_src = 
"precision mediump float;\n"
"#define tex2Dvs(s, t) texture2D(s, t)\n"
"#define mul(v,m) (m * v)\n"
"#define tex2D(s, t) texture2D(s, t)\n"
"#define static \n"
"#define _color gl_FragColor\n"
"#define texCUBE(s, t) textureCube(s, t)\n"
"#define _position gl_Position\n"
"varying vec2 uv;\n"
"void main() {\n"
"vec2 s0 = vec2(1.0 - 1.0/32767.0, 0.5 - 1.0/32767.0);\n"
"vec2 s1 = vec2(0.0, 0.5);\n"
"_color = vec4(s0, s1);\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* vsSynth_glsl120_src = 
"#version 120\n"
"#define tex2Dvs(s, t) texture2D(s, t)\n"
"#define mul(v,m) (m * v)\n"
"#define tex2D(s, t) texture2D(s, t)\n"
"#define static \n"
"#define _color gl_FragColor\n"
"#define texCUBE(s, t) textureCube(s, t)\n"
"#define _position gl_Position\n"
"attribute vec4 position;\n"
"void main() {\n"
"_position = position;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* fsSynth_glsl120_src = 
"#version 120\n"
"#define tex2Dvs(s, t) texture2D(s, t)\n"
"#define mul(v,m) (m * v)\n"
"#define tex2D(s, t) texture2D(s, t)\n"
"#define static \n"
"#define _color gl_FragColor\n"
"#define texCUBE(s, t) textureCube(s, t)\n"
"#define _position gl_Position\n"
"varying vec2 uv;\n"
"void main() {\n"
"vec2 s0 = vec2(1.0 - 1.0/32767.0, 0.5 - 1.0/32767.0);\n"
"vec2 s1 = vec2(0.0, 0.5);\n"
"_color = vec4(s0, s1);\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* vsSynth_glsl150_src = 
"#version 150\n"
"#define tex2Dvs(s, t) texture(s, t)\n"
"#define mul(v,m) (m * v)\n"
"#define tex2D(s, t) texture(s, t)\n"
"#define static \n"
"#define _color _FragColor\n"
"#define texCUBE(s, t) texture(s, t)\n"
"#define _position gl_Position\n"
"in vec4 position;\n"
"void main() {\n"
"_position = position;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* fsSynth_glsl150_src = 
"#version 150\n"
"#define tex2Dvs(s, t) texture(s, t)\n"
"#define mul(v,m) (m * v)\n"
"#define tex2D(s, t) texture(s, t)\n"
"#define static \n"
"#define _color _FragColor\n"
"#define texCUBE(s, t) texture(s, t)\n"
"#define _position gl_Position\n"
"in vec2 uv;\n"
"out vec4 _FragColor;\n"
"void main() {\n"
"vec2 s0 = vec2(1.0 - 1.0/32767.0, 0.5 - 1.0/32767.0);\n"
"vec2 s1 = vec2(0.0, 0.5);\n"
"_color = vec4(s0, s1);\n"
"}\n"
;
#endif
#if ORYOL_D3D11
const char* vsSynth_hlsl5_src = 
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
"in vec4 position : position,\n"
"out vec4 _oPosition : SV_POSITION) {\n"
"_position = position;\n"
"}\n"
;
#endif
#if ORYOL_D3D11
const char* fsSynth_hlsl5_src = 
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
"in vec2 uv : uv,\n"
"out vec4 _oColor : SV_TARGET) {\n"
"vec2 s0 = vec2(1.0 - 1.0/32767.0, 0.5 - 1.0/32767.0);\n"
"vec2 s1 = vec2(0.0, 0.5);\n"
"_color = vec4(s0, s1);\n"
"}\n"
;
#endif
ProgramBundleSetup Synth::CreateSetup() {
    ProgramBundleSetup setup("Synth");
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL100, vsSynth_glsl100_src, fsSynth_glsl100_src);
    #endif
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL120, vsSynth_glsl120_src, fsSynth_glsl120_src);
    #endif
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL150, vsSynth_glsl150_src, fsSynth_glsl150_src);
    #endif
    #if ORYOL_D3D11
    setup.AddProgramFromSources(0, ShaderLang::HLSL5, vsSynth_hlsl5_src, fsSynth_hlsl5_src);
    #endif
    return setup;
}
}
}

