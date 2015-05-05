//-----------------------------------------------------------------------------
// #version:15# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "shaders.h"

namespace Oryol {
namespace Shaders {
const char* normalsVS_100_src = 
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
const char* phongVS_100_src = 
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
const char* lambertVS_100_src = 
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
const char* normalsFS_100_src = 
"precision mediump float;\n"
"#define _COLOR gl_FragColor\n"
"varying vec4 nrm;\n"
"void main() {\n"
"_COLOR = nrm * 0.5 + 0.5;\n"
"}\n"
;
const char* phongFS_100_src = 
"precision mediump float;\n"
"#define _COLOR gl_FragColor\n"
"uniform vec3 lightDir;\n"
"uniform vec3 eyePos;\n"
"uniform vec4 lightColor;\n"
"uniform vec4 matDiffuse;\n"
"uniform vec4 matSpecular;\n"
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
"float n_dot_l = clamp(dot(n, l), 0.0, 1.0);\n"
"vec3 diffuse = matDiffuse.xyz * n_dot_l;\n"
"vec3 r = reflect(-l, n);\n"
"float r_dot_v = clamp(dot(r, v), 0.0, 1.0);\n"
"vec3 specular = matSpecular.xyz * pow(r_dot_v, matSpecularPower);\n"
"vec3 col = lightColor.xyz * (specular + diffuse);\n"
"_COLOR = gamma(vec4(col, 1.0));\n"
"}\n"
;
const char* lambertFS_100_src = 
"precision mediump float;\n"
"#define _COLOR gl_FragColor\n"
"uniform vec3 lightDir;\n"
"uniform vec4 lightColor;\n"
"uniform vec4 matDiffuse;\n"
"varying vec3 N;\n"
"vec4 gamma(vec4 c) {\n"
"return vec4(pow(c.xyz, vec3(1.0/2.2)), c.w);\n"
"}\n"
"void main() {\n"
"vec3 l = lightDir;\n"
"vec3 n = normalize(N);\n"
"float n_dot_l = clamp(dot(n, l), 0.0, 1.0);\n"
"_COLOR = gamma(vec4(lightColor.xyz * matDiffuse.xyz * n_dot_l, 1.0));\n"
"}\n"
;
const char* normalsVS_120_src = 
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
const char* phongVS_120_src = 
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
const char* lambertVS_120_src = 
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
const char* normalsFS_120_src = 
"#version 120\n"
"#define _COLOR gl_FragColor\n"
"varying vec4 nrm;\n"
"void main() {\n"
"_COLOR = nrm * 0.5 + 0.5;\n"
"}\n"
;
const char* phongFS_120_src = 
"#version 120\n"
"#define _COLOR gl_FragColor\n"
"uniform vec3 lightDir;\n"
"uniform vec3 eyePos;\n"
"uniform vec4 lightColor;\n"
"uniform vec4 matDiffuse;\n"
"uniform vec4 matSpecular;\n"
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
"float n_dot_l = clamp(dot(n, l), 0.0, 1.0);\n"
"vec3 diffuse = matDiffuse.xyz * n_dot_l;\n"
"vec3 r = reflect(-l, n);\n"
"float r_dot_v = clamp(dot(r, v), 0.0, 1.0);\n"
"vec3 specular = matSpecular.xyz * pow(r_dot_v, matSpecularPower);\n"
"vec3 col = lightColor.xyz * (specular + diffuse);\n"
"_COLOR = gamma(vec4(col, 1.0));\n"
"}\n"
;
const char* lambertFS_120_src = 
"#version 120\n"
"#define _COLOR gl_FragColor\n"
"uniform vec3 lightDir;\n"
"uniform vec4 lightColor;\n"
"uniform vec4 matDiffuse;\n"
"varying vec3 N;\n"
"vec4 gamma(vec4 c) {\n"
"return vec4(pow(c.xyz, vec3(1.0/2.2)), c.w);\n"
"}\n"
"void main() {\n"
"vec3 l = lightDir;\n"
"vec3 n = normalize(N);\n"
"float n_dot_l = clamp(dot(n, l), 0.0, 1.0);\n"
"_COLOR = gamma(vec4(lightColor.xyz * matDiffuse.xyz * n_dot_l, 1.0));\n"
"}\n"
;
const char* normalsVS_150_src = 
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
const char* phongVS_150_src = 
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
const char* lambertVS_150_src = 
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
const char* normalsFS_150_src = 
"#version 150\n"
"#define _COLOR _FragColor\n"
"in vec4 nrm;\n"
"out vec4 _FragColor;\n"
"void main() {\n"
"_COLOR = nrm * 0.5 + 0.5;\n"
"}\n"
;
const char* phongFS_150_src = 
"#version 150\n"
"#define _COLOR _FragColor\n"
"uniform vec3 lightDir;\n"
"uniform vec3 eyePos;\n"
"uniform vec4 lightColor;\n"
"uniform vec4 matDiffuse;\n"
"uniform vec4 matSpecular;\n"
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
"float n_dot_l = clamp(dot(n, l), 0.0, 1.0);\n"
"vec3 diffuse = matDiffuse.xyz * n_dot_l;\n"
"vec3 r = reflect(-l, n);\n"
"float r_dot_v = clamp(dot(r, v), 0.0, 1.0);\n"
"vec3 specular = matSpecular.xyz * pow(r_dot_v, matSpecularPower);\n"
"vec3 col = lightColor.xyz * (specular + diffuse);\n"
"_COLOR = gamma(vec4(col, 1.0));\n"
"}\n"
;
const char* lambertFS_150_src = 
"#version 150\n"
"#define _COLOR _FragColor\n"
"uniform vec3 lightDir;\n"
"uniform vec4 lightColor;\n"
"uniform vec4 matDiffuse;\n"
"in vec3 N;\n"
"out vec4 _FragColor;\n"
"vec4 gamma(vec4 c) {\n"
"return vec4(pow(c.xyz, vec3(1.0/2.2)), c.w);\n"
"}\n"
"void main() {\n"
"vec3 l = lightDir;\n"
"vec3 n = normalize(N);\n"
"float n_dot_l = clamp(dot(n, l), 0.0, 1.0);\n"
"_COLOR = gamma(vec4(lightColor.xyz * matDiffuse.xyz * n_dot_l, 1.0));\n"
"}\n"
;
ProgramBundleSetup Lambert::CreateSetup() {
    ProgramBundleSetup setup("Lambert");
    setup.AddProgramFromSources(0, ShaderLang::GLSL100, lambertVS_100_src, lambertFS_100_src);
    setup.AddProgramFromSources(0, ShaderLang::GLSL120, lambertVS_120_src, lambertFS_120_src);
    setup.AddProgramFromSources(0, ShaderLang::GLSL150, lambertVS_150_src, lambertFS_150_src);
    setup.AddUniform("mvp", ModelViewProjection);
    setup.AddUniform("model", Model);
    setup.AddUniform("lightDir", LightDir);
    setup.AddUniform("lightColor", LightColor);
    setup.AddUniform("matDiffuse", MatDiffuse);
    return setup;
}
ProgramBundleSetup Phong::CreateSetup() {
    ProgramBundleSetup setup("Phong");
    setup.AddProgramFromSources(0, ShaderLang::GLSL100, phongVS_100_src, phongFS_100_src);
    setup.AddProgramFromSources(0, ShaderLang::GLSL120, phongVS_120_src, phongFS_120_src);
    setup.AddProgramFromSources(0, ShaderLang::GLSL150, phongVS_150_src, phongFS_150_src);
    setup.AddUniform("mvp", ModelViewProjection);
    setup.AddUniform("model", Model);
    setup.AddUniform("lightDir", LightDir);
    setup.AddUniform("eyePos", EyePos);
    setup.AddUniform("lightColor", LightColor);
    setup.AddUniform("matDiffuse", MatDiffuse);
    setup.AddUniform("matSpecular", MatSpecular);
    setup.AddUniform("matSpecularPower", MatSpecularPower);
    return setup;
}
ProgramBundleSetup Normals::CreateSetup() {
    ProgramBundleSetup setup("Normals");
    setup.AddProgramFromSources(0, ShaderLang::GLSL100, normalsVS_100_src, normalsFS_100_src);
    setup.AddProgramFromSources(0, ShaderLang::GLSL120, normalsVS_120_src, normalsFS_120_src);
    setup.AddProgramFromSources(0, ShaderLang::GLSL150, normalsVS_150_src, normalsFS_150_src);
    setup.AddUniform("mvp", ModelViewProjection);
    return setup;
}
}
}

