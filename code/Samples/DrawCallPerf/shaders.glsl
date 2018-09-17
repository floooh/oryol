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
in vec4 in_pos;
in vec4 in_color;
out vec4 color;
void main() {
    gl_Position = mvp * (in_pos + translate);
    color = in_color;
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
