//-----------------------------------------------------------------------------
// #version:22# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "shaders.h"

namespace Oryol {
namespace Shaders {
#if ORYOL_OPENGL
const char* renderTargetVS_glsl100_src = 
"#define tex2Dvs(s, t) texture2D(s, t)\n"
"#define mul(v,m) (m * v)\n"
"#define tex2D(s, t) texture2D(s, t)\n"
"#define static \n"
"#define _color gl_FragColor\n"
"#define texCUBE(s, t) textureCube(s, t)\n"
"#define _position gl_Position\n"
"uniform mat4 mvp;\n"
"attribute vec4 position;\n"
"attribute vec4 normal;\n"
"varying vec4 nrm;\n"
"void main() {\n"
"_position = mul(position, mvp);\n"
"nrm = normal;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* mainVS_glsl100_src = 
"#define tex2Dvs(s, t) texture2D(s, t)\n"
"#define mul(v,m) (m * v)\n"
"#define tex2D(s, t) texture2D(s, t)\n"
"#define static \n"
"#define _color gl_FragColor\n"
"#define texCUBE(s, t) textureCube(s, t)\n"
"#define _position gl_Position\n"
"uniform mat4 mvp;\n"
"attribute vec4 position;\n"
"attribute vec4 normal;\n"
"attribute vec2 texcoord0;\n"
"varying vec4 nrm;\n"
"varying vec2 uv;\n"
"void main() {\n"
"_position = mul(position, mvp);\n"
"uv = texcoord0;\n"
"nrm = normalize(mul(normal, mvp));\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* renderTargetFS_glsl100_src = 
"precision mediump float;\n"
"#define tex2Dvs(s, t) texture2D(s, t)\n"
"#define mul(v,m) (m * v)\n"
"#define tex2D(s, t) texture2D(s, t)\n"
"#define static \n"
"#define _color gl_FragColor\n"
"#define texCUBE(s, t) textureCube(s, t)\n"
"#define _position gl_Position\n"
"varying vec4 nrm;\n"
"void main() {\n"
"_color = nrm * 0.5 + 0.5;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* mainFS_glsl100_src = 
"precision mediump float;\n"
"#define tex2Dvs(s, t) texture2D(s, t)\n"
"#define mul(v,m) (m * v)\n"
"#define tex2D(s, t) texture2D(s, t)\n"
"#define static \n"
"#define _color gl_FragColor\n"
"#define texCUBE(s, t) textureCube(s, t)\n"
"#define _position gl_Position\n"
"uniform sampler2D tex;\n"
"varying vec2 uv;\n"
"varying vec4 nrm;\n"
"void main() {\n"
"vec4 c = tex2D(tex, uv * vec2(20.0, 10.0));\n"
"float l = clamp(dot(nrm.xyz, normalize(vec3(1.0, 1.0, -1.0))), 0.0, 1.0) * 2.0;\n"
"_color = c * (l + 0.25);\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* renderTargetVS_glsl120_src = 
"#version 120\n"
"#define tex2Dvs(s, t) texture2D(s, t)\n"
"#define mul(v,m) (m * v)\n"
"#define tex2D(s, t) texture2D(s, t)\n"
"#define static \n"
"#define _color gl_FragColor\n"
"#define texCUBE(s, t) textureCube(s, t)\n"
"#define _position gl_Position\n"
"uniform mat4 mvp;\n"
"attribute vec4 position;\n"
"attribute vec4 normal;\n"
"varying vec4 nrm;\n"
"void main() {\n"
"_position = mul(position, mvp);\n"
"nrm = normal;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* mainVS_glsl120_src = 
"#version 120\n"
"#define tex2Dvs(s, t) texture2D(s, t)\n"
"#define mul(v,m) (m * v)\n"
"#define tex2D(s, t) texture2D(s, t)\n"
"#define static \n"
"#define _color gl_FragColor\n"
"#define texCUBE(s, t) textureCube(s, t)\n"
"#define _position gl_Position\n"
"uniform mat4 mvp;\n"
"attribute vec4 position;\n"
"attribute vec4 normal;\n"
"attribute vec2 texcoord0;\n"
"varying vec4 nrm;\n"
"varying vec2 uv;\n"
"void main() {\n"
"_position = mul(position, mvp);\n"
"uv = texcoord0;\n"
"nrm = normalize(mul(normal, mvp));\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* renderTargetFS_glsl120_src = 
"#version 120\n"
"#define tex2Dvs(s, t) texture2D(s, t)\n"
"#define mul(v,m) (m * v)\n"
"#define tex2D(s, t) texture2D(s, t)\n"
"#define static \n"
"#define _color gl_FragColor\n"
"#define texCUBE(s, t) textureCube(s, t)\n"
"#define _position gl_Position\n"
"varying vec4 nrm;\n"
"void main() {\n"
"_color = nrm * 0.5 + 0.5;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* mainFS_glsl120_src = 
"#version 120\n"
"#define tex2Dvs(s, t) texture2D(s, t)\n"
"#define mul(v,m) (m * v)\n"
"#define tex2D(s, t) texture2D(s, t)\n"
"#define static \n"
"#define _color gl_FragColor\n"
"#define texCUBE(s, t) textureCube(s, t)\n"
"#define _position gl_Position\n"
"uniform sampler2D tex;\n"
"varying vec2 uv;\n"
"varying vec4 nrm;\n"
"void main() {\n"
"vec4 c = tex2D(tex, uv * vec2(20.0, 10.0));\n"
"float l = clamp(dot(nrm.xyz, normalize(vec3(1.0, 1.0, -1.0))), 0.0, 1.0) * 2.0;\n"
"_color = c * (l + 0.25);\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* renderTargetVS_glsl150_src = 
"#version 150\n"
"#define tex2Dvs(s, t) texture(s, t)\n"
"#define mul(v,m) (m * v)\n"
"#define tex2D(s, t) texture(s, t)\n"
"#define static \n"
"#define _color _FragColor\n"
"#define texCUBE(s, t) texture(s, t)\n"
"#define _position gl_Position\n"
"uniform mat4 mvp;\n"
"in vec4 position;\n"
"in vec4 normal;\n"
"out vec4 nrm;\n"
"void main() {\n"
"_position = mul(position, mvp);\n"
"nrm = normal;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* mainVS_glsl150_src = 
"#version 150\n"
"#define tex2Dvs(s, t) texture(s, t)\n"
"#define mul(v,m) (m * v)\n"
"#define tex2D(s, t) texture(s, t)\n"
"#define static \n"
"#define _color _FragColor\n"
"#define texCUBE(s, t) texture(s, t)\n"
"#define _position gl_Position\n"
"uniform mat4 mvp;\n"
"in vec4 position;\n"
"in vec4 normal;\n"
"in vec2 texcoord0;\n"
"out vec4 nrm;\n"
"out vec2 uv;\n"
"void main() {\n"
"_position = mul(position, mvp);\n"
"uv = texcoord0;\n"
"nrm = normalize(mul(normal, mvp));\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* renderTargetFS_glsl150_src = 
"#version 150\n"
"#define tex2Dvs(s, t) texture(s, t)\n"
"#define mul(v,m) (m * v)\n"
"#define tex2D(s, t) texture(s, t)\n"
"#define static \n"
"#define _color _FragColor\n"
"#define texCUBE(s, t) texture(s, t)\n"
"#define _position gl_Position\n"
"in vec4 nrm;\n"
"out vec4 _FragColor;\n"
"void main() {\n"
"_color = nrm * 0.5 + 0.5;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* mainFS_glsl150_src = 
"#version 150\n"
"#define tex2Dvs(s, t) texture(s, t)\n"
"#define mul(v,m) (m * v)\n"
"#define tex2D(s, t) texture(s, t)\n"
"#define static \n"
"#define _color _FragColor\n"
"#define texCUBE(s, t) texture(s, t)\n"
"#define _position gl_Position\n"
"uniform sampler2D tex;\n"
"in vec2 uv;\n"
"in vec4 nrm;\n"
"out vec4 _FragColor;\n"
"void main() {\n"
"vec4 c = tex2D(tex, uv * vec2(20.0, 10.0));\n"
"float l = clamp(dot(nrm.xyz, normalize(vec3(1.0, 1.0, -1.0))), 0.0, 1.0) * 2.0;\n"
"_color = c * (l + 0.25);\n"
"}\n"
;
#endif
#if ORYOL_D3D11
const char* renderTargetVS_hlsl5_src = 
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
"void main(\n"
"in vec4 position : position,\n"
"in vec4 normal : normal,\n"
"out vec4 nrm : nrm,\n"
"out vec4 _oPosition : SV_POSITION) {\n"
"_position = mul(position, mvp);\n"
"nrm = normal;\n"
"}\n"
;
#endif
#if ORYOL_D3D11
const char* mainVS_hlsl5_src = 
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
"void main(\n"
"in vec4 position : position,\n"
"in vec4 normal : normal,\n"
"in vec2 texcoord0 : texcoord0,\n"
"out vec4 nrm : nrm,\n"
"out vec2 uv : uv,\n"
"out vec4 _oPosition : SV_POSITION) {\n"
"_position = mul(position, mvp);\n"
"uv = texcoord0;\n"
"nrm = normalize(mul(normal, mvp));\n"
"}\n"
;
#endif
#if ORYOL_D3D11
const char* renderTargetFS_hlsl5_src = 
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
"in vec4 nrm : nrm,\n"
"out vec4 _oColor : SV_TARGET) {\n"
"_color = nrm * 0.5 + 0.5;\n"
"}\n"
;
#endif
#if ORYOL_D3D11
const char* mainFS_hlsl5_src = 
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
"Texture2D tex;\n"
"SamplerState tex_sampler;\n"
"void main(\n"
"in vec2 uv : uv,\n"
"in vec4 nrm : nrm,\n"
"out vec4 _oColor : SV_TARGET) {\n"
"vec4 c = tex2D(tex, uv * vec2(20.0, 10.0));\n"
"float l = clamp(dot(nrm.xyz, normalize(vec3(1.0, 1.0, -1.0))), 0.0, 1.0) * 2.0;\n"
"_color = c * (l + 0.25);\n"
"}\n"
;
#endif
ProgramBundleSetup Main::CreateSetup() {
    ProgramBundleSetup setup("Main");
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL100, mainVS_glsl100_src, mainFS_glsl100_src);
    #endif
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL120, mainVS_glsl120_src, mainFS_glsl120_src);
    #endif
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL150, mainVS_glsl150_src, mainFS_glsl150_src);
    #endif
    #if ORYOL_D3D11
    setup.AddProgramFromSources(0, ShaderLang::HLSL5, mainVS_hlsl5_src, mainFS_hlsl5_src);
    #endif
    setup.AddUniform("mvp", ModelViewProjection);
    setup.AddTextureUniform("tex", Texture);
    return setup;
}
ProgramBundleSetup RenderTarget::CreateSetup() {
    ProgramBundleSetup setup("RenderTarget");
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL100, renderTargetVS_glsl100_src, renderTargetFS_glsl100_src);
    #endif
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL120, renderTargetVS_glsl120_src, renderTargetFS_glsl120_src);
    #endif
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL150, renderTargetVS_glsl150_src, renderTargetFS_glsl150_src);
    #endif
    #if ORYOL_D3D11
    setup.AddProgramFromSources(0, ShaderLang::HLSL5, renderTargetVS_hlsl5_src, renderTargetFS_hlsl5_src);
    #endif
    setup.AddUniform("mvp", ModelViewProjection);
    return setup;
}
}
}

