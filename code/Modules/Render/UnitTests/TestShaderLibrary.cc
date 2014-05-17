//-----------------------------------------------------------------------------
// #version:1# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "TestShaderLibrary.h"

namespace Oryol {
const char* TestShaderLibrary::MyVertexShader_src = 
"attribute vec4 position;\n"
"attribute vec4 normal;\n"
"attribute vec2 texcoord0;\n"
"varying vec4 nrm;\n"
"varying vec2 uv;\n"
"vec4 MyTransformFunc(in vec4 pos)\n"
"{\n"
"return mvp * pos;\n"
"}\n"
"void main()\n"
"{\n"
"RRRRRRRRRRR = MyTransformFunc(position);\n"
"nrm = normal;\n"
"uv  = texcoord0;\n"
"}\n";
const char* TestShaderLibrary::MyFragmentShader_src = 
"uniform sampler2D tex;\n" // Texture
"varying vec4 nrm;\n"
"varying vec2 uv;\n"
"vec4 MyColorFunc(in vec4 nrm,in vec4 texColor)\n"
"{\n"
"return ((nrm * 0.5) + 0.5) * 0.75 + texColor * texColor * texColor;\n"
"}\n"
"void main()\n"
"{\n"
"vec4 texColor = RRRRRRRRRRRR(tex, uv * vec2(5.0, 3.0));\n"
"RRRRRRRR = MyColorFunc(nrm, texColor);\n"
"}\n";
}

