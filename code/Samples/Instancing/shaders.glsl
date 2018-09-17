//------------------------------------------------------------------------------
//  Instancing sample shaders
//------------------------------------------------------------------------------

@vs vs
uniform vsParams {
    mat4 mvp;
};
in vec4 in_pos;
in vec4 in_color;
in vec4 in_instpos;
out vec4 color;
void main() {
    gl_Position = mvp * (in_pos + in_instpos);
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
