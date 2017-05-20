//------------------------------------------------------------------------------
//  Instancing sample shaders
//------------------------------------------------------------------------------

@vs vs
uniform vsParams {
    mat4 mvp;
};
in vec4 position;
in vec4 color0;
in vec4 instance0;
out vec4 color;
void main() {
    gl_Position = mvp * (position + instance0);
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
