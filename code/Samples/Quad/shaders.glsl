@vs vs
in vec4 position;
in vec4 color0;
out vec4 color;
void main() {
    gl_Position = position;
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
