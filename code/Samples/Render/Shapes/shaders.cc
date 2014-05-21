//-----------------------------------------------------------------------------
// #version:1# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "shaders.h"

namespace Oryol {
namespace Shaders{
const char* vs_100_src = 
"#define _POSITION gl_Position\n"
"uniform mat4 mvp;\n"
"attribute vec4 position;\n"
"attribute vec4 color0;\n"
"varying vec4 color;\n"
"void main() {\n"
"_POSITION = mvp * position;\n"
"color = color0;\n"
"}\n"
;
const char* fs_100_src = 
"precision mediump float;\n"
"#define _COLOR gl_FragColor\n"
"varying vec4 color;\n"
"void main() {\n"
"_COLOR = color;\n"
"}\n"
;
const char* vs_120_src = 
"#version 120\n"
"#define _POSITION gl_Position\n"
"uniform mat4 mvp;\n"
"attribute vec4 position;\n"
"attribute vec4 color0;\n"
"varying vec4 color;\n"
"void main() {\n"
"_POSITION = mvp * position;\n"
"color = color0;\n"
"}\n"
;
const char* fs_120_src = 
"#version 120\n"
"#define _COLOR gl_FragColor\n"
"varying vec4 color;\n"
"void main() {\n"
"_COLOR = color;\n"
"}\n"
;
const char* vs_150_src = 
"#version 150\n"
"#define _POSITION gl_Position\n"
"uniform mat4 mvp;\n"
"in vec4 position;\n"
"in vec4 color0;\n"
"out vec4 color;\n"
"void main() {\n"
"_POSITION = mvp * position;\n"
"color = color0;\n"
"}\n"
;
const char* fs_150_src = 
"#version 150\n"
"#define _COLOR _FragColor\n"
"in vec4 color;\n"
"out vec4 _FragColor;\n"
"void main() {\n"
"_COLOR = color;\n"
"}\n"
;
Render::ProgramBundleSetup Shapes::CreateSetup() {
    Render::ProgramBundleSetup setup("Shapes");
    setup.AddProgramFromSources(0, Render::ShaderLang::GLSL100, vs_100_src, fs_100_src);
    setup.AddProgramFromSources(0, Render::ShaderLang::GLSL120, vs_120_src, fs_120_src);
    setup.AddProgramFromSources(0, Render::ShaderLang::GLSL150, vs_150_src, fs_150_src);
    setup.AddUniform("mvp", ModelViewProjection);
    return setup;
}
}
}

