//-----------------------------------------------------------------------------
// #version:22# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "TestShaderLibrary.h"

namespace Oryol {
namespace Shaders {
#if ORYOL_OPENGL
const char* MyVertexShader_glsl100_src = 
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
"vec4 myTransform(vec4 pos)\n"
"{\n"
"return mvp * pos;\n"
"}\n"
"void main() {\n"
"_position = myTransform(position);\n"
"nrm = normal;\n"
"uv  = texcoord0;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* MyFragmentShader_glsl100_src = 
"precision mediump float;\n"
"#define tex2Dvs(s, t) texture2D(s, t)\n"
"#define mul(v,m) (m * v)\n"
"#define tex2D(s, t) texture2D(s, t)\n"
"#define static \n"
"#define _color gl_FragColor\n"
"#define texCUBE(s, t) textureCube(s, t)\n"
"#define _position gl_Position\n"
"uniform sampler2D tex;\n"
"varying vec4 nrm;\n"
"varying vec2 uv;\n"
"vec4 myColor(vec4 nrm, vec4 texColor)\n"
"{\n"
"return ((nrm * 0.5) + 0.5) * 0.75 + texColor * texColor * texColor;\n"
"}\n"
"void main() {\n"
"vec4 texColor = tex2D(tex, uv * vec2(5.0, 3.0));\n"
"_color = myColor(nrm, texColor);\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* MyVertexShader_glsl120_src = 
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
"vec4 myTransform(vec4 pos)\n"
"{\n"
"return mvp * pos;\n"
"}\n"
"void main() {\n"
"_position = myTransform(position);\n"
"nrm = normal;\n"
"uv  = texcoord0;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* MyFragmentShader_glsl120_src = 
"#version 120\n"
"#define tex2Dvs(s, t) texture2D(s, t)\n"
"#define mul(v,m) (m * v)\n"
"#define tex2D(s, t) texture2D(s, t)\n"
"#define static \n"
"#define _color gl_FragColor\n"
"#define texCUBE(s, t) textureCube(s, t)\n"
"#define _position gl_Position\n"
"uniform sampler2D tex;\n"
"varying vec4 nrm;\n"
"varying vec2 uv;\n"
"vec4 myColor(vec4 nrm, vec4 texColor)\n"
"{\n"
"return ((nrm * 0.5) + 0.5) * 0.75 + texColor * texColor * texColor;\n"
"}\n"
"void main() {\n"
"vec4 texColor = tex2D(tex, uv * vec2(5.0, 3.0));\n"
"_color = myColor(nrm, texColor);\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* MyVertexShader_glsl150_src = 
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
"vec4 myTransform(vec4 pos)\n"
"{\n"
"return mvp * pos;\n"
"}\n"
"void main() {\n"
"_position = myTransform(position);\n"
"nrm = normal;\n"
"uv  = texcoord0;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* MyFragmentShader_glsl150_src = 
"#version 150\n"
"#define tex2Dvs(s, t) texture(s, t)\n"
"#define mul(v,m) (m * v)\n"
"#define tex2D(s, t) texture(s, t)\n"
"#define static \n"
"#define _color _FragColor\n"
"#define texCUBE(s, t) texture(s, t)\n"
"#define _position gl_Position\n"
"uniform sampler2D tex;\n"
"in vec4 nrm;\n"
"in vec2 uv;\n"
"out vec4 _FragColor;\n"
"vec4 myColor(vec4 nrm, vec4 texColor)\n"
"{\n"
"return ((nrm * 0.5) + 0.5) * 0.75 + texColor * texColor * texColor;\n"
"}\n"
"void main() {\n"
"vec4 texColor = tex2D(tex, uv * vec2(5.0, 3.0));\n"
"_color = myColor(nrm, texColor);\n"
"}\n"
;
#endif
#if ORYOL_D3D11
const char* MyVertexShader_hlsl5_src = 
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
"vec4 myTransform(vec4 pos)\n"
"{\n"
"return mvp * pos;\n"
"}\n"
"void main(\n"
"in vec4 position : position,\n"
"in vec4 normal : normal,\n"
"in vec2 texcoord0 : texcoord0,\n"
"out vec4 nrm : nrm,\n"
"out vec2 uv : uv,\n"
"out vec4 _oPosition : SV_POSITION) {\n"
"_position = myTransform(position);\n"
"nrm = normal;\n"
"uv  = texcoord0;\n"
"}\n"
;
#endif
#if ORYOL_D3D11
const char* MyFragmentShader_hlsl5_src = 
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
"vec4 myColor(vec4 nrm, vec4 texColor)\n"
"{\n"
"return ((nrm * 0.5) + 0.5) * 0.75 + texColor * texColor * texColor;\n"
"}\n"
"void main(\n"
"in vec4 nrm : nrm,\n"
"in vec2 uv : uv,\n"
"out vec4 _oColor : SV_TARGET) {\n"
"vec4 texColor = tex2D(tex, uv * vec2(5.0, 3.0));\n"
"_color = myColor(nrm, texColor);\n"
"}\n"
;
#endif
ProgramBundleSetup MyShader::CreateSetup() {
    ProgramBundleSetup setup("MyShader");
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL100, MyVertexShader_glsl100_src, MyFragmentShader_glsl100_src);
    #endif
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL120, MyVertexShader_glsl120_src, MyFragmentShader_glsl120_src);
    #endif
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL150, MyVertexShader_glsl150_src, MyFragmentShader_glsl150_src);
    #endif
    #if ORYOL_D3D11
    setup.AddProgramFromSources(0, ShaderLang::HLSL5, MyVertexShader_hlsl5_src, MyFragmentShader_hlsl5_src);
    #endif
    setup.AddUniform("mvp", ModelViewProj);
    setup.AddTextureUniform("tex", Texture);
    return setup;
}
}
}

