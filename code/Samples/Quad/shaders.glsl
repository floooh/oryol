@vs vs
in vec4 in_pos;
in vec4 in_color;
out vec4 color;
void main() {
    gl_Position = in_pos;
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
