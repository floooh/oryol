//-----------------------------------------------------------------------------
// #version:17# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "shaders.h"

namespace Oryol {
namespace Shaders {
#if ORYOL_OPENGL
const char* normalsVS_glsl100_src = 
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
#endif
#if ORYOL_OPENGL
const char* phongVS_glsl100_src = 
"#define _POSITION gl_Position\n"
"uniform mat4 mvp;\n"
"uniform mat4 model;\n"
"attribute vec4 position;\n"
"attribute vec4 normal;\n"
"varying vec3 N;\n"
"varying vec3 P;\n"
"void main() {\n"
"_POSITION = mvp * position;\n"
"P = vec4(model * position).xyz;\n"
"N = vec4(model * vec4(normal.xyz, 0.0)).xyz;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* lambertVS_glsl100_src = 
"#define _POSITION gl_Position\n"
"uniform mat4 mvp;\n"
"uniform mat4 model;\n"
"attribute vec4 position;\n"
"attribute vec3 normal;\n"
"varying vec3 N;\n"
"void main() {\n"
"_POSITION = mvp * position;\n"
"N = vec4(model * vec4(normal, 0.0)).xyz;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* normalsFS_glsl100_src = 
"precision mediump float;\n"
"#define _COLOR gl_FragColor\n"
"varying vec4 nrm;\n"
"void main() {\n"
"_COLOR = nrm * 0.5 + 0.5;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* phongFS_glsl100_src = 
"precision mediump float;\n"
"#define _COLOR gl_FragColor\n"
"uniform vec3 lightDir;\n"
"uniform vec3 eyePos;\n"
"uniform vec4 lightColor;\n"
"uniform vec4 matDiffuse;\n"
"uniform vec4 matSpecular;\n"
"uniform bool gammaCorrect;\n"
"uniform float matSpecularPower;\n"
"varying vec3 P;\n"
"varying vec3 N;\n"
"vec4 gamma(vec4 c) {\n"
"return vec4(pow(c.xyz, vec3(1.0/2.2)), c.w);\n"
"}\n"
"void main() {\n"
"vec3 l = lightDir;\n"
"vec3 n = normalize(N);\n"
"vec3 v = normalize(eyePos - P);\n"
"float n_dot_l = max(dot(n, l), 0.0);\n"
"vec3 r = reflect(-l, n);\n"
"float r_dot_v = max(dot(r, v), 0.0);\n"
"float diff = n_dot_l;\n"
"float spec = pow(r_dot_v, matSpecularPower) * n_dot_l;\n"
"vec4 col = vec4(lightColor.xyz * (matSpecular.xyz*spec + matDiffuse.xyz*diff), 1.0);\n"
"if (gammaCorrect) {\n"
"_COLOR = gamma(col);\n"
"} else {\n"
"_COLOR = col;\n"
"}\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* lambertFS_glsl100_src = 
"precision mediump float;\n"
"#define _COLOR gl_FragColor\n"
"uniform vec3 lightDir;\n"
"uniform vec4 lightColor;\n"
"uniform vec4 matDiffuse;\n"
"uniform bool gammaCorrect;\n"
"varying vec3 N;\n"
"vec4 gamma(vec4 c) {\n"
"return vec4(pow(c.xyz, vec3(1.0/2.2)), c.w);\n"
"}\n"
"void main() {\n"
"vec3 l = lightDir;\n"
"vec3 n = normalize(N);\n"
"float n_dot_l = max(dot(n, l), 0.0);\n"
"vec4 col = vec4(lightColor.xyz * matDiffuse.xyz * n_dot_l, 1.0);\n"
"if (gammaCorrect) {\n"
"_COLOR = gamma(col);\n"
"}\n"
"else {\n"
"_COLOR = col;\n"
"}\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* normalsVS_glsl120_src = 
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
#endif
#if ORYOL_OPENGL
const char* phongVS_glsl120_src = 
"#version 120\n"
"#define _POSITION gl_Position\n"
"uniform mat4 mvp;\n"
"uniform mat4 model;\n"
"attribute vec4 position;\n"
"attribute vec4 normal;\n"
"varying vec3 N;\n"
"varying vec3 P;\n"
"void main() {\n"
"_POSITION = mvp * position;\n"
"P = vec4(model * position).xyz;\n"
"N = vec4(model * vec4(normal.xyz, 0.0)).xyz;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* lambertVS_glsl120_src = 
"#version 120\n"
"#define _POSITION gl_Position\n"
"uniform mat4 mvp;\n"
"uniform mat4 model;\n"
"attribute vec4 position;\n"
"attribute vec3 normal;\n"
"varying vec3 N;\n"
"void main() {\n"
"_POSITION = mvp * position;\n"
"N = vec4(model * vec4(normal, 0.0)).xyz;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* normalsFS_glsl120_src = 
"#version 120\n"
"#define _COLOR gl_FragColor\n"
"varying vec4 nrm;\n"
"void main() {\n"
"_COLOR = nrm * 0.5 + 0.5;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* phongFS_glsl120_src = 
"#version 120\n"
"#define _COLOR gl_FragColor\n"
"uniform vec3 lightDir;\n"
"uniform vec3 eyePos;\n"
"uniform vec4 lightColor;\n"
"uniform vec4 matDiffuse;\n"
"uniform vec4 matSpecular;\n"
"uniform bool gammaCorrect;\n"
"uniform float matSpecularPower;\n"
"varying vec3 P;\n"
"varying vec3 N;\n"
"vec4 gamma(vec4 c) {\n"
"return vec4(pow(c.xyz, vec3(1.0/2.2)), c.w);\n"
"}\n"
"void main() {\n"
"vec3 l = lightDir;\n"
"vec3 n = normalize(N);\n"
"vec3 v = normalize(eyePos - P);\n"
"float n_dot_l = max(dot(n, l), 0.0);\n"
"vec3 r = reflect(-l, n);\n"
"float r_dot_v = max(dot(r, v), 0.0);\n"
"float diff = n_dot_l;\n"
"float spec = pow(r_dot_v, matSpecularPower) * n_dot_l;\n"
"vec4 col = vec4(lightColor.xyz * (matSpecular.xyz*spec + matDiffuse.xyz*diff), 1.0);\n"
"if (gammaCorrect) {\n"
"_COLOR = gamma(col);\n"
"} else {\n"
"_COLOR = col;\n"
"}\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* lambertFS_glsl120_src = 
"#version 120\n"
"#define _COLOR gl_FragColor\n"
"uniform vec3 lightDir;\n"
"uniform vec4 lightColor;\n"
"uniform vec4 matDiffuse;\n"
"uniform bool gammaCorrect;\n"
"varying vec3 N;\n"
"vec4 gamma(vec4 c) {\n"
"return vec4(pow(c.xyz, vec3(1.0/2.2)), c.w);\n"
"}\n"
"void main() {\n"
"vec3 l = lightDir;\n"
"vec3 n = normalize(N);\n"
"float n_dot_l = max(dot(n, l), 0.0);\n"
"vec4 col = vec4(lightColor.xyz * matDiffuse.xyz * n_dot_l, 1.0);\n"
"if (gammaCorrect) {\n"
"_COLOR = gamma(col);\n"
"}\n"
"else {\n"
"_COLOR = col;\n"
"}\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* normalsVS_glsl150_src = 
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
#endif
#if ORYOL_OPENGL
const char* phongVS_glsl150_src = 
"#version 150\n"
"#define _POSITION gl_Position\n"
"uniform mat4 mvp;\n"
"uniform mat4 model;\n"
"in vec4 position;\n"
"in vec4 normal;\n"
"out vec3 N;\n"
"out vec3 P;\n"
"void main() {\n"
"_POSITION = mvp * position;\n"
"P = vec4(model * position).xyz;\n"
"N = vec4(model * vec4(normal.xyz, 0.0)).xyz;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* lambertVS_glsl150_src = 
"#version 150\n"
"#define _POSITION gl_Position\n"
"uniform mat4 mvp;\n"
"uniform mat4 model;\n"
"in vec4 position;\n"
"in vec3 normal;\n"
"out vec3 N;\n"
"void main() {\n"
"_POSITION = mvp * position;\n"
"N = vec4(model * vec4(normal, 0.0)).xyz;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* normalsFS_glsl150_src = 
"#version 150\n"
"#define _COLOR _FragColor\n"
"in vec4 nrm;\n"
"out vec4 _FragColor;\n"
"void main() {\n"
"_COLOR = nrm * 0.5 + 0.5;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* phongFS_glsl150_src = 
"#version 150\n"
"#define _COLOR _FragColor\n"
"uniform vec3 lightDir;\n"
"uniform vec3 eyePos;\n"
"uniform vec4 lightColor;\n"
"uniform vec4 matDiffuse;\n"
"uniform vec4 matSpecular;\n"
"uniform bool gammaCorrect;\n"
"uniform float matSpecularPower;\n"
"in vec3 P;\n"
"in vec3 N;\n"
"out vec4 _FragColor;\n"
"vec4 gamma(vec4 c) {\n"
"return vec4(pow(c.xyz, vec3(1.0/2.2)), c.w);\n"
"}\n"
"void main() {\n"
"vec3 l = lightDir;\n"
"vec3 n = normalize(N);\n"
"vec3 v = normalize(eyePos - P);\n"
"float n_dot_l = max(dot(n, l), 0.0);\n"
"vec3 r = reflect(-l, n);\n"
"float r_dot_v = max(dot(r, v), 0.0);\n"
"float diff = n_dot_l;\n"
"float spec = pow(r_dot_v, matSpecularPower) * n_dot_l;\n"
"vec4 col = vec4(lightColor.xyz * (matSpecular.xyz*spec + matDiffuse.xyz*diff), 1.0);\n"
"if (gammaCorrect) {\n"
"_COLOR = gamma(col);\n"
"} else {\n"
"_COLOR = col;\n"
"}\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* lambertFS_glsl150_src = 
"#version 150\n"
"#define _COLOR _FragColor\n"
"uniform vec3 lightDir;\n"
"uniform vec4 lightColor;\n"
"uniform vec4 matDiffuse;\n"
"uniform bool gammaCorrect;\n"
"in vec3 N;\n"
"out vec4 _FragColor;\n"
"vec4 gamma(vec4 c) {\n"
"return vec4(pow(c.xyz, vec3(1.0/2.2)), c.w);\n"
"}\n"
"void main() {\n"
"vec3 l = lightDir;\n"
"vec3 n = normalize(N);\n"
"float n_dot_l = max(dot(n, l), 0.0);\n"
"vec4 col = vec4(lightColor.xyz * matDiffuse.xyz * n_dot_l, 1.0);\n"
"if (gammaCorrect) {\n"
"_COLOR = gamma(col);\n"
"}\n"
"else {\n"
"_COLOR = col;\n"
"}\n"
"}\n"
;
#endif
ProgramBundleSetup Lambert::CreateSetup() {
    ProgramBundleSetup setup("Lambert");
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL100, lambertVS_glsl100_src, lambertFS_glsl100_src);
    #endif
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL120, lambertVS_glsl120_src, lambertFS_glsl120_src);
    #endif
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL150, lambertVS_glsl150_src, lambertFS_glsl150_src);
    #endif
    #if ORYOL_D3D11
    setup.AddProgramFromSources(0, ShaderLang::HLSL5, lambertVS_hlsl5_src, lambertFS_hlsl5_src);
    #endif
    setup.AddUniform("mvp", ModelViewProjection);
    setup.AddUniform("model", Model);
    setup.AddUniform("lightDir", LightDir);
    setup.AddUniform("lightColor", LightColor);
    setup.AddUniform("matDiffuse", MatDiffuse);
    setup.AddUniform("gammaCorrect", GammaCorrect);
    return setup;
}
ProgramBundleSetup Phong::CreateSetup() {
    ProgramBundleSetup setup("Phong");
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL100, phongVS_glsl100_src, phongFS_glsl100_src);
    #endif
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL120, phongVS_glsl120_src, phongFS_glsl120_src);
    #endif
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL150, phongVS_glsl150_src, phongFS_glsl150_src);
    #endif
    #if ORYOL_D3D11
    setup.AddProgramFromSources(0, ShaderLang::HLSL5, phongVS_hlsl5_src, phongFS_hlsl5_src);
    #endif
    setup.AddUniform("mvp", ModelViewProjection);
    setup.AddUniform("model", Model);
    setup.AddUniform("lightDir", LightDir);
    setup.AddUniform("eyePos", EyePos);
    setup.AddUniform("lightColor", LightColor);
    setup.AddUniform("matDiffuse", MatDiffuse);
    setup.AddUniform("matSpecular", MatSpecular);
    setup.AddUniform("gammaCorrect", GammaCorrect);
    setup.AddUniform("matSpecularPower", MatSpecularPower);
    return setup;
}
ProgramBundleSetup Normals::CreateSetup() {
    ProgramBundleSetup setup("Normals");
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL100, normalsVS_glsl100_src, normalsFS_glsl100_src);
    #endif
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL120, normalsVS_glsl120_src, normalsFS_glsl120_src);
    #endif
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL150, normalsVS_glsl150_src, normalsFS_glsl150_src);
    #endif
    #if ORYOL_D3D11
    setup.AddProgramFromSources(0, ShaderLang::HLSL5, normalsVS_hlsl5_src, normalsFS_hlsl5_src);
    #endif
    setup.AddUniform("mvp", ModelViewProjection);
    return setup;
}
}
}

