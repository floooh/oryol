//-----------------------------------------------------------------------------
// #version:5# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "SynthShaders.h"

namespace Oryol {
namespace Shaders{
const char* vsSynth_100_src = 
"#define _POSITION gl_Position\n"
"attribute vec4 position;\n"
"void main() {\n"
"_POSITION = position;\n"
"}\n"
;
const char* fsSynth_100_src = 
"precision mediump float;\n"
"#define _COLOR gl_FragColor\n"
"varying vec2 uv;\n"
"void main() {\n"
"vec2 s0 = vec2(1.0 - 1.0/32767.0, 0.5 - 1.0/32767.0);\n"
"vec2 s1 = vec2(0.0, 0.5);\n"
"_COLOR = vec4(s0, s1);\n"
"}\n"
;
const char* vsSynth_120_src = 
"#version 120\n"
"#define _POSITION gl_Position\n"
"attribute vec4 position;\n"
"void main() {\n"
"_POSITION = position;\n"
"}\n"
;
const char* fsSynth_120_src = 
"#version 120\n"
"#define _COLOR gl_FragColor\n"
"varying vec2 uv;\n"
"void main() {\n"
"vec2 s0 = vec2(1.0 - 1.0/32767.0, 0.5 - 1.0/32767.0);\n"
"vec2 s1 = vec2(0.0, 0.5);\n"
"_COLOR = vec4(s0, s1);\n"
"}\n"
;
const char* vsSynth_150_src = 
"#version 150\n"
"#define _POSITION gl_Position\n"
"in vec4 position;\n"
"void main() {\n"
"_POSITION = position;\n"
"}\n"
;
const char* fsSynth_150_src = 
"#version 150\n"
"#define _COLOR _FragColor\n"
"in vec2 uv;\n"
"out vec4 _FragColor;\n"
"void main() {\n"
"vec2 s0 = vec2(1.0 - 1.0/32767.0, 0.5 - 1.0/32767.0);\n"
"vec2 s1 = vec2(0.0, 0.5);\n"
"_COLOR = vec4(s0, s1);\n"
"}\n"
;
ProgramBundleSetup Synth::CreateSetup() {
    ProgramBundleSetup setup("Synth");
    setup.AddProgramFromSources(0, ShaderLang::GLSL100, vsSynth_100_src, fsSynth_100_src);
    setup.AddProgramFromSources(0, ShaderLang::GLSL120, vsSynth_120_src, fsSynth_120_src);
    setup.AddProgramFromSources(0, ShaderLang::GLSL150, vsSynth_150_src, fsSynth_150_src);
    return setup;
}
}
}

