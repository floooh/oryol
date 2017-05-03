//------------------------------------------------------------------------------
//  DrawCallPerf sample shaders
//------------------------------------------------------------------------------

@vs vs
uniform perFrameParams {
    mat4 mvp;
};
uniform perParticleParams {
    vec4 translate;
};
in vec4 position;
in vec4 color0;
out vec4 color;
void main() {
    gl_Position = mvp * (position + translate);
    color = color0;
}
@end

@fs fs
in vec4 color;
out vec4 fragColor;
void main() {
    fragColor = color;
}
@end

@program Shader vs fs
